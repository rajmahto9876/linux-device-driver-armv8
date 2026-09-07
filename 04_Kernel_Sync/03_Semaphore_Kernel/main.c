#include <linux/module.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/kthread.h>

#define ENABLE_MUTEX 	1

#if ENABLE_MUTEX
	#include <linux/mutex.h>
	DEFINE_MUTEX(kernel_mutex_lock);
#endif

struct task_struct *thread_1 = NULL;
struct task_struct *thread_2 = NULL;
static int counter = 0;

int func_1(void *args)
{
	while(!kthread_should_stop())
	{
		#if ENABLE_MUTEX == 1
			mutex_lock(&kernel_mutex_lock);
			counter = counter + 1;
			pr_info("From Func_1 = %d \n", counter);
			mutex_unlock(&kernel_mutex_lock);
		#else
			counter = counter + 1;
			pr_info("From Func_1 = %d \n", counter);
		#endif
		msleep(1);
	}
	return 0;
}

int func_2(void *args)
{
	while(!kthread_should_stop())
	{
		#if ENABLE_MUTEX == 1
			mutex_lock(&kernel_mutex_lock);
			counter = counter + 1;
			pr_info("From Func_2 = %d \n", counter);
			mutex_unlock(&kernel_mutex_lock);
		#else
			counter = counter + 1;
			pr_info("From Func_2 = %d \n", counter);
		#endif
		
		msleep(1);
	}
	
	return 0;
}

static int __init mutex_driver_init(void)
{
    pr_info("Mutex Driver Init \n");

    thread_1 = kthread_create(func_1, NULL, "thread_%d", 1);
    if(IS_ERR(thread_1))
    {
    	pr_err("kthread1 Create Error \n");
    	return  PTR_ERR(thread_1);
    }

    thread_2 = kthread_create(func_2, NULL, "thread_%d", 2);
    if(IS_ERR(thread_2))
    {
    	pr_err("kthread2 Create Error \n");
    	return  PTR_ERR(thread_2);
    }

    wake_up_process(thread_1);
    wake_up_process(thread_2);

    return 0;
}

static void __exit mutex_driver_clean(void)
{
	pr_info("Mutex Driver Test Exit \n");

	pr_info("counter_Value %d", counter);
	kthread_stop(thread_1);
	kthread_stop(thread_2);
}

module_init(mutex_driver_init);
module_exit(mutex_driver_clean);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Raj Kumar Mahto");
MODULE_DESCRIPTION("A mutex test");
