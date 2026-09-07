#include<linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <asm/io.h>
#undef pr_fmt
#define pr_fmt(fmt)             "%s : "fmt,__func__

static char *buffer1 = NULL;
static char *buffer2 = NULL;

static int __init module_driver_init(void)
{
    phys_addr_t phys1;
    phys_addr_t phys2;
    
    pr_info("Module Driver Init Called \n");

    buffer1 = vmalloc(4096, GFP_KERNEL);
    if (!buffer1)
    {
        pr_err("kmalloc_lab: allocation failed\n");
        return -ENOMEM;
    }

    buffer2 = vmalloc(8192, GFP_KERNEL);
    if (!buffer2)
    {
        pr_err("kmalloc_lab: allocation failed\n");
        return -ENOMEM;
    }
    phys1 = virt_to_phys(buffer1);
    phys2 = virt_to_phys(buffer2);

    pr_info("kmalloc_lab: virtual address1 = %px\n", buffer1);
    pr_info("kmalloc_lab: physical address1 = %llx\n", (unsigned long long)phys1);

    pr_info("kmalloc_lab: virtual address2 = %px\n", buffer2);
    pr_info("kmalloc_lab: physical address2 = %llx\n", (unsigned long long)phys2);
    return 0;
}

static void __exit module_driver_clean(void)
{
    pr_info("kmalloc_lab: freeing memory\n");
    
    if (!buffer2)
    {
        vfree(buffer2);
    }

    if (!buffer1)
    {
        vfree(buffer1);
    }
    
    pr_info("Module Driver Clean Up Called \n");
}

module_init(module_driver_init);
module_exit(module_driver_clean);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Raj Kumar Mahto");
MODULE_DESCRIPTION("A simple character driver module for memory check");




