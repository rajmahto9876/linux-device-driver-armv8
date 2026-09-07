// SPDX-License-Identifier: GPL-2.0
/*
 * irq_demo.c - top-half / bottom-half interrupt handling demo
 *
 * Real hardware path (preferred -- e.g. BeagleBone Black with a GPIO
 * loopback wire, see EXERCISES.md for wiring instructions, or any
 * board where you can obtain a real IRQ number):
 *
 *   sudo insmod irq_demo.ko irq_num=<N>
 *
 * Fallback path (no hardware wired -- e.g. plain QEMU):
 *
 *   sudo insmod irq_demo.ko          # irq_num defaults to -1
 *
 *   A periodic hrtimer fires in hardirq context and drives the same
 *   "top half" code path a real ISR would run through, so atomic
 *   context rules (might_sleep() splats etc) are genuinely exercised
 *   even with no line wired up. The "bottom half" runs on a workqueue
 *   in this mode, since there is no real IRQ thread without
 *   request_threaded_irq().
 *
 * Either way:
 *   - event_count is incremented under a spinlock in the top half
 *   - the bottom half signals a completion
 *   - cat /proc/irq_demo blocks on that completion, reports state
 *   - echo 1 > /proc/irq_demo_trigger fires one event on demand
 *     (process context only -- useful for sanity-checking logic,
 *     NOT a substitute for real-IRQ or hrtimer atomic-context testing)
 */

#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/hrtimer.h>
#include <linux/workqueue.h>
#include <linux/completion.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include <linux/sched.h>

#define DRV_NAME "irq_demo"

static int irq_num = -1;
module_param(irq_num, int, 0444);
MODULE_PARM_DESC(irq_num, "Real IRQ number to attach to. -1 = hrtimer fallback simulator.");

static DEFINE_SPINLOCK(irq_lock);
static unsigned long event_count;
static struct completion event_done;

static struct hrtimer fallback_timer;
static struct work_struct fallback_bottom_half;
static bool using_fallback;

/* ---------------------------------------------------------------
 * Shared top-half / bottom-half bodies. Both the real-IRQ path and
 * the hrtimer-fallback path funnel through these two functions, so
 * the atomic-context rules you're testing are identical either way.
 * ------------------------------------------------------------- */

/*
 * Runs in hardirq context in BOTH modes: either the real ISR, or the
 * hrtimer callback (hrtimers fire in hardirq context). Must obey every
 * rule a hard-IRQ handler obeys: no sleeping, no GFP_KERNEL alloc,
 * no non-irqsafe locks.
 */
static void do_hard_isr_work(void)
{
	unsigned long flags;

	spin_lock_irqsave(&irq_lock, flags);
	event_count++;
	spin_unlock_irqrestore(&irq_lock, flags);

	/* pr_info() does not sleep -- safe here. Do NOT add msleep(),
	 * mutex_lock(), or kmalloc(GFP_KERNEL) to this function. */
	pr_info(DRV_NAME ": top half, count=%lu in_hardirq=%d preempt_count=0x%08x\n",
		event_count, (int)in_hardirq(), preempt_count());
}

/*
 * Runs in process context in BOTH modes: either the request_threaded_irq()
 * kernel thread, or a workqueue worker. Sleeping is legal here.
 */
static void do_bottom_half_work(void)
{
	pr_info(DRV_NAME ": bottom half start, current=%s/%d in_interrupt=%d\n",
		current->comm, current->pid, (int)in_interrupt());

	msleep(50); /* stand-in for real deferred work -- legal ONLY here */

	pr_info(DRV_NAME ": bottom half done, signaling completion\n");
	complete(&event_done);
}

/* ---------------- real hardware path ---------------- */

static irqreturn_t hard_isr(int irq, void *dev_id)
{
	do_hard_isr_work();
	return IRQ_WAKE_THREAD;
}

static irqreturn_t threaded_isr(int irq, void *dev_id)
{
	do_bottom_half_work();
	return IRQ_HANDLED;
}

/* ---------------- hrtimer fallback path ---------------- */

static void fallback_workqueue_fn(struct work_struct *w)
{
	do_bottom_half_work();
}

static enum hrtimer_restart fallback_timer_fn(struct hrtimer *t)
{
	do_hard_isr_work();
	schedule_work(&fallback_bottom_half); /* stand-in for IRQ_WAKE_THREAD */
	hrtimer_forward_now(t, ms_to_ktime(5000));
	return HRTIMER_RESTART;
}

/* ---------------- /proc interface ---------------- */

static ssize_t irq_demo_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
	char msg[96];
	int n;
	unsigned long flags, count;

	if (*off > 0)
		return 0;

	pr_info(DRV_NAME ": /proc read waiting for next event...\n");
	if (wait_for_completion_interruptible_timeout(&event_done, msecs_to_jiffies(10000)) <= 0)
		return -ETIMEDOUT;

	spin_lock_irqsave(&irq_lock, flags);
	count = event_count;
	spin_unlock_irqrestore(&irq_lock, flags);

	n = scnprintf(msg, sizeof(msg), "events=%lu mode=%s\n",
		      count, using_fallback ? "hrtimer-fallback" : "real-irq");
	if (copy_to_user(buf, msg, n))
		return -EFAULT;
	*off += n;
	return n;
}

static ssize_t irq_demo_trigger_write(struct file *f, const char __user *buf,
				       size_t len, loff_t *off)
{
	/* Software-triggered firing from process context. Good for
	 * sanity-checking logic before real hardware is wired up, but
	 * NOT a substitute for proving atomic-context correctness --
	 * only a real ISR or the hrtimer fallback actually runs this
	 * in hardirq context. */
	do_hard_isr_work();
	schedule_work(&fallback_bottom_half);
	return len;
}

static const struct proc_ops irq_demo_fops = { .proc_read = irq_demo_read };
static const struct proc_ops irq_demo_trigger_fops = { .proc_write = irq_demo_trigger_write };

static int __init irq_demo_init(void)
{
	int ret;

	init_completion(&event_done);
	INIT_WORK(&fallback_bottom_half, fallback_workqueue_fn);

	proc_create(DRV_NAME, 0444, NULL, &irq_demo_fops);
	proc_create(DRV_NAME "_trigger", 0222, NULL, &irq_demo_trigger_fops);

	if (irq_num >= 0) {
		using_fallback = false;
		ret = request_threaded_irq(irq_num, hard_isr, threaded_isr,
					    IRQF_TRIGGER_RISING | IRQF_ONESHOT,
					    DRV_NAME, NULL);
		if (ret) {
			pr_err(DRV_NAME ": request_threaded_irq(%d) failed: %d\n", irq_num, ret);
			remove_proc_entry(DRV_NAME, NULL);
			remove_proc_entry(DRV_NAME "_trigger", NULL);
			return ret;
		}
		pr_info(DRV_NAME ": attached to real irq %d\n", irq_num);
	} else {
		using_fallback = true;
		hrtimer_init(&fallback_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
		fallback_timer.function = fallback_timer_fn;
		hrtimer_start(&fallback_timer, ms_to_ktime(5000), HRTIMER_MODE_REL);
		pr_info(DRV_NAME ": no irq_num given, using hrtimer fallback (fires every 5s)\n");
	}

	return 0;
}

static void __exit irq_demo_exit(void)
{
	if (!using_fallback) {
		/* free_irq() blocks until any in-flight hard handler and
		 * threaded handler have both finished before returning --
		 * see EXERCISES.md for what happens if you get this
		 * ordering wrong relative to freeing device state. */
		free_irq(irq_num, NULL);
	} else {
		hrtimer_cancel(&fallback_timer);
	}
	cancel_work_sync(&fallback_bottom_half);

	remove_proc_entry(DRV_NAME, NULL);
	remove_proc_entry(DRV_NAME "_trigger", NULL);
}

module_init(irq_demo_init);
module_exit(irq_demo_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Top-half/bottom-half IRQ demo with real-hardware and hrtimer-fallback paths");
