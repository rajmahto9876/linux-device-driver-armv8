#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/kdev_t.h>
#include<linux/uaccess.h>

#undef pr_fmt
#define pr_fmt(fmt)             "%s : "fmt,__func__

#define MAX_DEVICE_COUNT	7
#define DVC_MEM_SIZE    	512

/* Pseudo Character Buffer */
char dvc_buf[DVC_MEM_SIZE];

/*Device Number */
dev_t dvc_num;

/*cdev struct */
struct cdev char_cdev;

/* pointer to class */
struct class *pchar_Class;

/* pointer to device under the class */
struct device *pDvc;

/* file-ops Callbacks */
static loff_t char_lseek(struct file *pFops, loff_t offset, int whence)
{
    loff_t temp;
    pr_info("lseek_called \n");
    pr_info("Current File Pos %lld \n", pFops->f_pos);
    
    switch(whence)
    {
    	case SEEK_SET:
	{
	    if((offset > DVC_MEM_SIZE) || (offset < 0))
	    {
                 return -EINVAL;
	    }
	    pFops->f_pos = offset;
	}
	break;
	
	case SEEK_CUR:
        {
	    temp = pFops->f_pos + offset;
            if((temp > DVC_MEM_SIZE) || (temp < 0))
            {
                 return -EINVAL;
            }
            pFops->f_pos = temp;
        }
        break;

	case SEEK_END:
        {
	    temp = DVC_MEM_SIZE + offset;
            if((temp > DVC_MEM_SIZE) || (temp < 0))
            {
                 return -EINVAL;
            }
            pFops->f_pos = temp;
        }
        break;
	
	default:
		return -EINVAL;
    }

    pr_info("Update File pos %lld \n", pFops->f_pos);
    return 0;
}

static ssize_t char_read(struct file *pFops, char __user *buff, size_t count, loff_t *pFpos)
{
    pr_info("char_read requested %zu bytes\n", count);
    pr_info("current file pos = %lld \n", *pFpos);

    /* 1. Adjust the count should not be greater then MEM_SIZE*/
    if((*pFpos + count)> DVC_MEM_SIZE)
    {
        count = DVC_MEM_SIZE - *pFpos;
    }

    /* 2. Copy to user buffer */
    if(copy_to_user(buff, &dvc_buf[*pFpos], count))
    {
    	return -EFAULT;
    }
    
    /* update the *pFpos */
    *pFpos = *pFpos + count;

    pr_info("no of bytes succesfully read %zu \n", count);
    pr_info("Update file pos %lld \n", *pFpos);
    return count;
}

static ssize_t char_write(struct file *pFops, const  char __user *buff, size_t count, loff_t *pFpos)
{
    pr_info("char_write requested %zu bytes\n", count);
    pr_info("current file pos = %lld \n", *pFpos);

    /* 1. Adjust the count should not be greater then MEM_SIZE*/
    if((*pFpos + count)> DVC_MEM_SIZE)
    {
        count = DVC_MEM_SIZE - *pFpos;
    }

    if(!count)
    {
	return -ENOMEM;
    }

    /* 2. Copy from user buffer */
    if(copy_from_user(&dvc_buf[*pFpos], buff, count))
    {
        return -EFAULT;
    }

    /* update the *pFpos */
    *pFpos = *pFpos + count;

    pr_info("no of bytes succesfully written %zu \n", count);
    pr_info("Update file pos %lld \n", *pFpos);
    return count;
}

static int char_open(struct inode *pInode, struct file *pFops)
{
   pr_info("char_open called \n");
   return 0;
}

static int char_flush(struct file *pFops, fl_owner_t id)
{
    pr_info("char_flush called \n");
    return 0;
}

static int char_release(struct inode *pInode, struct file *pFops)
{
    pr_info("char_release called \n");
    return 0;
}

/*file-ops struct */
struct file_operations char_fops = 
{
     .llseek  = char_lseek,
     .read    = char_read,
     .write   = char_write,
     .open    = char_open,
     .flush   = char_flush,
     .release = char_release,
     .owner   = THIS_MODULE
};


static int __init char_driver_init(void)
{
    pr_info("Driver Init Called \n");

    /* 1. Dynamically allocate device number */
    alloc_chrdev_region(&dvc_num, 0, MAX_DEVICE_COUNT, "chr_dev_sample");

    pr_info("<Major>:<Minor> = %d:%d \n", MAJOR(dvc_num), MINOR(dvc_num)); 

    /* 2. Initilise cdev with file operations */
    cdev_init(&char_cdev, &char_fops);

    /* 3. Register cdev with VFS */
    char_cdev.owner = THIS_MODULE;
    cdev_add(&char_cdev, dvc_num, MAX_DEVICE_COUNT);

    /* 4. Class Create under /sys/class */
    pchar_Class = class_create("char_class");
    
    /* 5. Device Create under the Class */
    pDvc = device_create(pchar_Class, NULL, dvc_num, NULL, "char_dvd");

    pr_info("Driver Init OK\n");
    return 0;
}

static void __exit char_driver_clean(void)
{
    /* 1. Destroy Device */
    device_destroy(pchar_Class, dvc_num);
    
    /* 2. Class Destroy */
    class_destroy(pchar_Class);

    /* 3. Cdev Delete */
    cdev_del(&char_cdev);

    /* 4. Unallocate device Number */
    unregister_chrdev_region(dvc_num, MAX_DEVICE_COUNT);

    pr_info("Driver Clean Up Called \n");
}

module_init(char_driver_init);
module_exit(char_driver_clean);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Raj Kumar Mahto");
MODULE_DESCRIPTION("A simple character driver module");




