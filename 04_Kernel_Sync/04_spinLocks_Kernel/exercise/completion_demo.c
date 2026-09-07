// SPDX-License-Identifier: GPL-2.0
/*
 * completion_demo.c - Two completion patterns
 *
 * Pattern 1: single-shot "device ready" signal.
 *   A kthread does slow init work, then complete()s. A /proc reader
 *   blocks in wait_for_completion() until that happens. If the reader
 *   arrives AFTER complete() has already run, it does not block --
 *   done > 0 already, this is the property that makes completions
 *   safe against the lost-wakeup race a hand-rolled flag+wake_up()
 *   does not have.
 *
 * Pattern 2: complete_all() broadcast to N waiters.
 *   Several simulated "waiter" kthreads block on the same completion.
 *   Writing to /proc/completion_demo_release wakes ALL of them at once
 *   via complete_all(), as opposed to complete() which wakes exactly one.
 *
 * Usage:
 *   insmod completion_demo.ko
 *   cat /proc/completion_demo            # blocks until worker finishes init
 *   dmesg -w                             # watch the 4 release-waiters block
 *   echo 1 > /proc/completion_demo_release  # releases all of them at once
 */

#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/completion.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#define DRV_NAME "completion_demo"
#define NUM_WAITERS 4

/* Pattern 1: single-shot device-ready completion */
static struct completion init_done;
static struct task_struct *worker_thread;

/* Pattern 2: complete_all broadcast */
static struct completion release_gate;
static struct task_struct *waiters[NUM_WAITERS];

static int worker_fn(void *data)
{
	pr_info(DRV_NAME ": worker starting slow init\n");
	msleep(3000);
	pr_info(DRV_NAME ": worker init finished, calling complete()\n");
	complete(&init_done);

	while (!kthread_should_stop())
		msleep(1000);
	return 0;
}

static int waiter_fn(void *data)
{
	long id = (long)data;

	pr_info(DRV_NAME ": waiter %ld blocking on release_gate\n", id);
	wait_for_completion(&release_gate);
	pr_info(DRV_NAME ": waiter %ld released\n", id);

	while (!kthread_should_stop())
		msleep(1000);
	return 0;
}

static ssize_t status_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
	char msg[32];
	int n;

	if (*off > 0)
		return 0;

	pr_info(DRV_NAME ": /proc read waiting on init_done\n");
	wait_for_completion(&init_done);
	pr_info(DRV_NAME ": /proc read woke up\n");

	n = scnprintf(msg, sizeof(msg), "device ready\n");
	if (copy_to_user(buf, msg, n))
		return -EFAULT;
	*off += n;
	return n;
}

static ssize_t release_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
	pr_info(DRV_NAME ": releasing all waiters via complete_all()\n");
	complete_all(&release_gate);
	return len;
}

static const struct proc_ops status_fops = { .proc_read = status_read };
static const struct proc_ops release_fops = { .proc_write = release_write };

static int __init completion_demo_init(void)
{
	long i;

	init_completion(&init_done);
	init_completion(&release_gate);

	proc_create(DRV_NAME, 0444, NULL, &status_fops);
	proc_create(DRV_NAME "_release", 0222, NULL, &release_fops);

	worker_thread = kthread_run(worker_fn, NULL, "comp_worker");

	for (i = 0; i < NUM_WAITERS; i++)
		waiters[i] = kthread_run(waiter_fn, (void *)i, "comp_waiter%ld", i);

	return 0;
}

static void __exit completion_demo_exit(void)
{
	long i;

	kthread_stop(worker_thread);
	for (i = 0; i < NUM_WAITERS; i++)
		kthread_stop(waiters[i]);

	remove_proc_entry(DRV_NAME, NULL);
	remove_proc_entry(DRV_NAME "_release", NULL);
}

module_init(completion_demo_init);
module_exit(completion_demo_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("completion() vs complete_all() demo");
