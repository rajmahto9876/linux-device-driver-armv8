/*
 * sem_demo.c — Practice module for struct semaphore (counting semaphore)
 *
 * Scenario:
 *   - We simulate a fixed-size pool of N "buffers" (just integers 0..N-1).
 *   - A kernel worker thread (producer) periodically "produces" data and
 *     needs a free buffer from the pool -> down(&free_slots).
 *   - A /proc write handler (consumer, process context, triggered by you
 *     doing `echo 1 > /proc/sem_demo`) "consumes" a filled buffer ->
 *     down(&filled_slots).
 *   - Two semaphores model classic producer/consumer:
 *       free_slots   : starts at POOL_SIZE  (how many empty slots exist)
 *       filled_slots : starts at 0          (how many filled slots exist)
 *   - A mutex protects the actual buffer array/indices (semaphores handle
 *     counting, mutex handles the critical section on shared state).
 *
 * Build:  make
 * Load:   sudo insmod sem_demo.ko
 * Watch:  dmesg -w
 * Consume one item:  echo 1 | sudo tee /proc/sem_demo
 * Unload: sudo rmmod sem_demo
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/semaphore.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/delay.h>

#define PROC_NAME "sem_demo"
#define POOL_SIZE 4

static struct semaphore free_slots;    /* counts empty buffer slots */
static struct semaphore filled_slots;  /* counts filled buffer slots */
static struct mutex buf_lock;          /* protects buffer[] and indices */

static int buffer[POOL_SIZE];
static int prod_idx;   /* next slot producer will fill */
static int cons_idx;   /* next slot consumer will drain */
static int produced_count;
static int consumed_count;

static struct task_struct *producer_thread;
static struct proc_dir_entry *proc_entry;

/* ---- Producer: runs in its own kernel thread (process context) ---- */
static int producer_fn(void *data)
{
	int value = 0;

	while (!kthread_should_stop()) {
		/*
		 * Block until there's a free slot. down_interruptible() lets
		 * this thread be killed cleanly (e.g. via kthread_stop(),
		 * which sends a wake via kthread_should_stop() + a signal-like
		 * mechanism) instead of being stuck uninterruptibly forever.
		 */
		if (down_interruptible(&free_slots)) {
			pr_info("sem_demo: producer interrupted, exiting\n");
			break;
		}

		mutex_lock(&buf_lock);
		buffer[prod_idx] = value;
		pr_info("sem_demo: PRODUCED value=%d into slot=%d\n",
			value, prod_idx);
		prod_idx = (prod_idx + 1) % POOL_SIZE;
		produced_count++;
		value++;
		mutex_unlock(&buf_lock);

		/* Signal that a filled slot is now available */
		up(&filled_slots);

		msleep(500); /* simulate work between productions */
	}

	return 0;
}

/* ---- Consumer: triggered from userspace via `echo 1 > /proc/sem_demo` ---- */
static ssize_t demo_proc_write(struct file *file, const char __user *ubuf,
				size_t count, loff_t *ppos)
{
	int consumed_value;

	/*
	 * EXERCISE 1: try down() instead of down_interruptible() here and
	 * observe `ps -eo pid,stat,comm | grep sem_demo`-style state (well,
	 * this is a syscall context so check the calling process, e.g. your
	 * `tee` process) go into D state if you write when nothing has been
	 * produced yet. With plain down(), Ctrl+C on `tee` won't unblock it.
	 */
	if (down_interruptible(&filled_slots))
		return -ERESTARTSYS;

	mutex_lock(&buf_lock);
	consumed_value = buffer[cons_idx];
	pr_info("sem_demo: CONSUMED value=%d from slot=%d\n",
		consumed_value, cons_idx);
	cons_idx = (cons_idx + 1) % POOL_SIZE;
	consumed_count++;
	mutex_unlock(&buf_lock);

	/* Freed up a slot for the producer */
	up(&free_slots);

	return count; /* pretend we consumed the whole write */
}

static ssize_t demo_proc_read(struct file *file, char __user *ubuf,
			       size_t count, loff_t *ppos)
{
	char kbuf[128];
	int len;

	if (*ppos > 0)
		return 0;

	mutex_lock(&buf_lock);
	len = scnprintf(kbuf, sizeof(kbuf),
			"produced=%d consumed=%d free_slots=%d filled_slots=%d\n",
			produced_count, consumed_count,
			free_slots.count, filled_slots.count);
	mutex_unlock(&buf_lock);

	return simple_read_from_buffer(ubuf, count, ppos, kbuf, len);
}

static const struct proc_ops demo_proc_ops = {
	.proc_read = demo_proc_read,
	.proc_write = demo_proc_write,
};

static int __init sem_demo_init(void)
{
	sema_init(&free_slots, POOL_SIZE); /* all slots start empty/free */
	sema_init(&filled_slots, 0);       /* nothing produced yet */
	mutex_init(&buf_lock);

	prod_idx = cons_idx = produced_count = consumed_count = 0;

	proc_entry = proc_create(PROC_NAME, 0666, NULL, &demo_proc_ops);
	if (!proc_entry) {
		pr_err("sem_demo: failed to create /proc/%s\n", PROC_NAME);
		return -ENOMEM;
	}

	producer_thread = kthread_run(producer_fn, NULL, "sem_demo_producer");
	if (IS_ERR(producer_thread)) {
		proc_remove(proc_entry);
		return PTR_ERR(producer_thread);
	}

	pr_info("sem_demo: loaded. cat /proc/%s for stats, "
		"echo 1 > /proc/%s to consume one item\n",
		PROC_NAME, PROC_NAME);
	return 0;
}

static void __exit sem_demo_exit(void)
{
	kthread_stop(producer_thread);
	proc_remove(proc_entry);
	pr_info("sem_demo: unloaded. produced=%d consumed=%d\n",
		produced_count, consumed_count);
}

module_init(sem_demo_init);
module_exit(sem_demo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("practice");
MODULE_DESCRIPTION("Semaphore practice: producer/consumer buffer pool");
