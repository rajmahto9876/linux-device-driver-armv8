#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

#define MAX_KERNEL_BUFFER   256
#define DVC_BASE_MINOR      0
#define DVC_MAX_COUNT       7

static char kernel_buffer[MAX_KERNEL_BUFFER];
static dev_t dvc_number = 0;
const char *dvc_number_name = "dvc_num";

static struct  class *dvc_class = NULL;
const char *class_name = "dvc_class";

static struct cdev dvc_cdev;

static struct device * dvc_device = NULL;
const char *device_name = "dvc_device";

/* file-ops Callbacks */
static loff_t dev_lseek(struct file *pFops, loff_t offset, int whence)
{
    pr_info("lseek_called \n");
    return 0;
}

static ssize_t dev_read(struct file *pFops, char __user *buff, 
            size_t count, loff_t *pFpos)
{
    int ret = 0;
    ssize_t read_bytes = 0;
    pr_info("dev_read requested %zu bytes\n", count);
    pr_info("Current File Pos = [%lld] \n", (*pFpos));

    /* count + current count > size*/
    read_bytes = (*pFpos) + count;
    if( read_bytes> MAX_KERNEL_BUFFER)
    {
        read_bytes = MAX_KERNEL_BUFFER - (*pFpos);
    }

    if(read_bytes < 0 || read_bytes == 0)
    {
        pr_err("Negative read_bytes \n");
        return -ENOMEM;
    }

    ret = copy_to_user(buff, kernel_buffer + (*pFpos), read_bytes);
    if(ret!= 0)
    {
        pr_err("Failed to Copy [%ld] bytes", ret);
        read_bytes = read_bytes - ret;
    }

    *pFpos = *pFpos + read_bytes;
    pr_info("No of Bytes written = [%ld]\n", read_bytes);
    pr_info("Updated File Pos = [%lld]\n", (*pFpos));
    return read_bytes;
}

static ssize_t dev_write(struct file *pFops, const  char __user *buff, 
            size_t count, loff_t *pFpos)
{
    int ret = 0;
    ssize_t write_bytes = 0;
    pr_info("dev_write requested %zu bytes\n", count);
    pr_info("Current File Pos = [%lld] \n", (*pFpos));

    /* count + current count > size*/
    write_bytes = (*pFpos) + count;
    if( write_bytes> MAX_KERNEL_BUFFER)
    {
        write_bytes = MAX_KERNEL_BUFFER - (*pFpos);
    }

    if(write_bytes < 0 || write_bytes == 0)
    {
        pr_err("Negative Write Bytes \n");
        return -ENOMEM;
    }

    ret = copy_from_user(kernel_buffer + (*pFpos), buff, write_bytes);
    if(ret!= 0)
    {
        pr_err("Failed to Copy [%ld] bytes", ret);
        write_bytes = write_bytes - ret;
    }

    *pFpos = *pFpos + write_bytes;
    pr_info("No of Bytes written = [%ld]\n", write_bytes);
    pr_info("Updated File Pos = [%lld]\n", (*pFpos));
    return write_bytes;
}

static int dev_open(struct inode *pInode, struct file *pFops)
{
   pr_info("dev_open called \n");
   return 0;
}

static int dev_flush(struct file *pFops, fl_owner_t id)
{
    pr_info("dev_flush called \n");
    return 0;
}

static int dev_release(struct inode *pInode, struct file *pFops)
{
    pr_info("dev_release called \n");
    return 0;
}

const struct file_operations dvc_fops =
{
    .llseek  = dev_lseek,
    .read    = dev_read,
    .write   = dev_write,
    .open    = dev_open,
    .flush   = dev_flush,
    .release = dev_release,
    .owner   = THIS_MODULE
};

static int __init dev_driver_init(void) 
{
    int ret = 0;
    /* 1. allocate Device Number */
    ret = alloc_chrdev_region(&dvc_number, DVC_BASE_MINOR, DVC_MAX_COUNT,
            dvc_number_name);
    if(ret!= 0)
    {
        pr_err("Alloc Char dev Failed");
        goto init_end;
    }
    pr_info("Dvc Num = [%d], Major = [%d], Minor = [%d] \n", dvc_number, MAJOR(dvc_number), MINOR(dvc_number));

    /* 2. Allocate Device Class */
    dvc_class = class_create(THIS_MODULE, class_name);
    if(IS_ERR(dvc_class))
    {
        pr_err("Class Create Error \n");
        ret = PTR_ERR(dvc_class);
        goto unregister_chrdev;
    }

    /* 3. Allocate Cdev (fops) */
    cdev_init(&dvc_cdev, &dvc_fops);
    dvc_cdev.owner = THIS_MODULE;

    ret = cdev_add(&dvc_cdev, dvc_number, DVC_MAX_COUNT);
    if(ret!= 0)
    {
        pr_err("cdev_add Error \n");
        goto class_deinit;
    }

    /* 4. Device Create */
    dvc_device = device_create(dvc_class, NULL, dvc_number, NULL, device_name);
    if(IS_ERR(dvc_device))
    {
        pr_err("Device Create Error \n");
        ret = PTR_ERR(dvc_device);
        goto cdev_deinit;
    }

    pr_info("In Init character Driver\n");
    return 0;

cdev_deinit:
    cdev_del(&dvc_cdev);

class_deinit:
    class_destroy(dvc_class);

unregister_chrdev:
    unregister_chrdev_region(dvc_number, DVC_MAX_COUNT);

init_end:
    return ret;
}

static void __exit dev_driver_clean(void) 
{

    if(dvc_device!= NULL)
    {
        device_destroy(dvc_class, dvc_number);
    }

    cdev_del(&dvc_cdev);

    if(dvc_class!= NULL)
    {
        class_destroy(dvc_class);
    }

    if(dvc_number!= 0)
    {
        unregister_chrdev_region(dvc_number, DVC_MAX_COUNT);
    }
    
    pr_info("In Exit character Driver\n");
}

module_init(dev_driver_init);
module_exit(dev_driver_clean);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Raj Kumar Mahto");
MODULE_DESCRIPTION("A simple character driver module");




