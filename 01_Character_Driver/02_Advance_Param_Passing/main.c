/*
# Write a character driver that supports 2 independent devices

    /dev/mydev0
    /dev/mydev1

# Each device should maintain its own:
    struct my_device {
    struct cdev cdev;
    int value;
    char name[20];
};
*/

#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

#define NUMBER_OF_DVC       3
#define DVC_BASE_MINOR      0
#define DVC_MAX_COUNT       NUMBER_OF_DVC

/* pseudo device's memory */
const char *drv_class_name   = "drv_class";
const char *drv_dvc_name_prf = "drv_dvc";
const char *drv_dvc_num_name = "drv_num";

#define MAX_DVC_1_BUFFER        256
#define MAX_DVC_2_BUFFER        1024 
char dvc_buff_dev1[MAX_DVC_1_BUFFER];
char dvc_buff_dev2[MAX_DVC_2_BUFFER];


/*Device private data structure */
struct dev_pvt_data_t
{
    char *buffer;
    unsigned size;
    const char *idt_num;
    struct cdev dvc_cdev;
};

/* Driver private data structure */
struct drv_pvt_data_t
{
    int total_devices;
    /* This holds the device number */
    dev_t  dvc_number;
    struct class *dvc_class;
    struct device *dvc_device;
    struct dev_pvt_data_t dev_data[NUMBER_OF_DVC];
};

struct drv_pvt_data_t drv_pvt_data = 
{
    .total_devices = NUMBER_OF_DVC,
    .dvc_number = 0,
    .dvc_class = NULL,
    .dvc_device = NULL,
    .dev_data[0] =
    {
        .buffer  = dvc_buff_dev1,
        .size    = MAX_DVC_1_BUFFER,
        .idt_num = "dvc_1",
    },

    .dev_data[1] =
    {
        .buffer  = dvc_buff_dev2,
        .size    = MAX_DVC_2_BUFFER,
        .idt_num = "dvc_2",
    },
};

/* file-ops Callbacks */
static loff_t dev_lseek(struct file *pFops, loff_t offset, int whence)
{
    #if 0
    int ret = 0;
    loff_t temp_pos = 0;
    pr_info("Current File Pos [%lld]\n", pFops->f_pos);
    switch(whence)
    {
        /* to set postion to Offset. */
        case SEEK_SET:
        {
            pr_info("In SEEK_SET \n");
            if((offset > MAX_KERNEL_BUFFER) || (offset < 0))
            {
                return -EINVAL;
            }
            pFops->f_pos = offset;
        }
        break;

        /* to set postion with reference to Current. */
        case SEEK_CUR: 
        {
            pr_info("In SEEK_CUR \n");
            temp_pos = pFops->f_pos + offset;
            if((temp_pos > MAX_KERNEL_BUFFER) || (temp_pos < 0))
            {
                return -EINVAL;
            }
            pFops->f_pos = temp_pos;
        }
        break;

        /* to set postion with reference to End (-v2 value is passed in offset). */
        case SEEK_END:
        {
            pr_info("In SEEK_END \n");
            temp_pos = MAX_KERNEL_BUFFER + offset;
            if((temp_pos > MAX_KERNEL_BUFFER) || (temp_pos < 0))
            {
                return -EINVAL;
            }
            pFops->f_pos = temp_pos;
        }
        break;

        default:
        {
            pr_info("In Invalid Case \n");
            return -EINVAL;
        }
        break;
    }

    pr_info("Updated File Pos [%lld]\n", pFops->f_pos);
    return pFops->f_pos;
    #endif
    return 0;
}

static ssize_t dev_read(struct file *pFops, char __user *buff, 
            size_t count, loff_t *pFpos)
{
    #if 0
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
    #endif
    return 0;
}

static ssize_t dev_write(struct file *pFops, const  char __user *buff, 
            size_t count, loff_t *pFpos)
{
    #if 0
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
    #endif

    return 0;
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
    int itr = 0;
    /* 1. allocate Device Number */
    ret = alloc_chrdev_region(&drv_pvt_data.dvc_number, DVC_BASE_MINOR, DVC_MAX_COUNT,
            drv_dvc_num_name);
    if(ret!= 0)
    {
        pr_err("Alloc Char dev Failed");
        goto init_end;
    }

    pr_info("Dvc Num = [%d], Major = [%d], Minor = [%d] \n", drv_pvt_data.dvc_number, MAJOR(drv_pvt_data.dvc_number), MINOR(drv_pvt_data.dvc_number));

    /* 2. Allocate Device Class */
    drv_pvt_data.dvc_class = class_create(THIS_MODULE, drv_class_name);
    if(IS_ERR(drv_pvt_data.dvc_class))
    {
        pr_err("Class Create Error \n");
        ret = PTR_ERR(drv_pvt_data.dvc_class);
        goto unregister_chrdev;
    }

    for(itr = 0; itr < DVC_MAX_COUNT; itr++)
    {
        /* 3. Allocate Cdev (fops) */
        cdev_init(&drv_pvt_data.dev_data[itr].dvc_cdev, &dvc_fops);
        drv_pvt_data.dev_data[itr].dvc_cdev.owner = THIS_MODULE;

        ret = cdev_add(&drv_pvt_data.dev_data[itr].dvc_cdev, drv_pvt_data.dvc_number + itr, 1);
        if(ret!= 0)
        {
            pr_err("cdev_add Error at itr [%d]\n", itr);
            goto class_deinit;
        }

        /* 4. Device Create */
        drv_pvt_data.dvc_device = device_create(drv_pvt_data.dvc_class, NULL, drv_pvt_data.dvc_number + itr, 
            NULL, drv_dvc_name_prf, "%d", itr + 1);

        if(IS_ERR(drv_pvt_data.dvc_device))
        {
            pr_err("Device Create Error at itr [%d]\n", itr);
            ret = PTR_ERR(drv_pvt_data.dvc_device);
            goto cdev_deinit;
        }
    }
    

    pr_info("In Init character Driver\n");
    return 0;

cdev_deinit:
    for(;itr>=0; itr--)
    {
        device_destroy(drv_pvt_data.dvc_class, drv_pvt_data.dvc_number + itr);
        cdev_del(&drv_pvt_data.dev_data[itr].dvc_cdev);
    }

class_deinit:
    class_destroy(drv_pvt_data.dvc_class);

unregister_chrdev:
    unregister_chrdev_region(drv_pvt_data.dvc_number, DVC_MAX_COUNT);

init_end:
    return ret;
}

static void __exit dev_driver_clean(void) 
{
    int itr = 0;
    for(itr = 0; itr<DVC_MAX_COUNT; itr++)
    {
        if(drv_pvt_data.dvc_device!= NULL)
        {
            device_destroy(drv_pvt_data.dvc_class, drv_pvt_data.dvc_number + itr);
            cdev_del(&drv_pvt_data.dev_data[itr].dvc_cdev);
        }
    }

    if(drv_pvt_data.dvc_class!= NULL)
    {
        class_destroy(drv_pvt_data.dvc_class);
    }

    if(drv_pvt_data.dvc_number!= 0)
    {
        unregister_chrdev_region(drv_pvt_data.dvc_number, DVC_MAX_COUNT);
    }
    
    pr_info("In Exit character Driver\n");
}

module_init(dev_driver_init);
module_exit(dev_driver_clean);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Raj Kumar Mahto");
MODULE_DESCRIPTION("A simple character driver module");
