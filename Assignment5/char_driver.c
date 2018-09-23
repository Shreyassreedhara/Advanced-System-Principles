#include <linux/version.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/list.h>
#include <linux/string.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/err.h>

#define MYDEV_NAME "mycdev"
#define ramdisk_size (size_t) (16 * PAGE_SIZE) // ramdisk size 
#define CDEV_IOC_MAGIC 'k'
#define ASP_CLEAR_BUFF _IO(CDEV_IOC_MAGIC, 1)

static int NUM_DEVICES = 3;

struct ASP_mycdev 
{
	struct cdev cdev;
	char *ramdisk;
	unsigned long buffer_size;
	struct semaphore sem;
	int devNo;
	struct list_head list;
};

// NUM_DEVICES cannot be changed by the world
module_param(NUM_DEVICES, int, S_IRUGO);
static unsigned int majornum = 0;
static struct class *dev_group = NULL;
// initialize the kernel linked list
LIST_HEAD(dev_head);

int mycdev_open(struct inode *inode, struct file *filp)
{
        unsigned int mjr = imajor(inode);
        unsigned int mnr = iminor(inode);
	struct list_head *curr = NULL;
        struct ASP_mycdev *dev = NULL;

	if (mjr != majornum || mnr < 0 || mnr >= NUM_DEVICES) 
	{
		// Error in major or minor numberor minor number is less than NUM_DEVICES
                printk("No device found with minor=%d and major=%d\n", mnr, mjr);
                return -ENODEV;
        }

	list_for_each(curr, &dev_head) {
		dev = list_entry(curr, struct ASP_mycdev, list);
		if(dev->devNo == mnr) 
		{
			// Linked list has reached the end of the devices
			break;
		}
	}

        filp->private_data = dev; 

        return 0;
}

ssize_t mycdev_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	struct ASP_mycdev *dev = (struct ASP_mycdev *)filp->private_data;
	ssize_t retval = 0;
	unsigned char *tmp_buff = NULL;
	loff_t f;

	if (down_interruptible(&(dev->sem))!=0) 
	{
		// The lock has been acquired to do either write, close or lseek operation
		pr_err("%s: could not lock during open\n", MYDEV_NAME);
	}

	if (*f_pos >= dev->buffer_size) /* EOF */
		goto brkpnt;

	tmp_buff = (unsigned char*) kzalloc(count, GFP_KERNEL);	
	if (tmp_buff == NULL) 
	{
		pr_err("%s: failed due to out of memory operation", __func__);
		retval = -ENOMEM;
		goto brkpnt;
	}

	for (f = 0; f < count; f++)
	{
		tmp_buff[f] = dev->ramdisk[*f_pos + f];
	}
	
	*f_pos += count;

	if (copy_to_user(buf, tmp_buff, count) != 0)
	{
		retval = -EFAULT;
		goto brkpnt;
	}

	retval = count;

brkpnt:
	if (tmp_buff != NULL)
		kfree(tmp_buff);
	up(&(dev->sem));
	return retval;
}

long mycdev_ioctl (struct file *filp, unsigned int lok, unsigned long argmnts)
{
        struct ASP_mycdev *dev = (struct ASP_mycdev *)filp->private_data;
	long ret;

	if (lok != ASP_CLEAR_BUFF) 
	{
		pr_err("command: %d: act_command: %d\n", lok, ASP_CLEAR_BUFF);
		return -1;
	}

	if (down_interruptible(&(dev->sem))!=0) {
		pr_err("%s: could not lock during open\n", MYDEV_NAME);
	} 

	//memset(void* ptr, int value, size_t num)	
	memset((volatile void*)dev->ramdisk,0,dev->buffer_size);
	filp->f_pos=0;	

	up(&(dev->sem));
	return 1;
}

ssize_t mycdev_write(struct file *filp, const char __user *buf, size_t count, 
		loff_t *f_pos)
{
	struct ASP_mycdev *dev = (struct ASP_mycdev *)filp->private_data;
	ssize_t retval = 0;
	unsigned char *tmp_buff = NULL;
	loff_t f;

	if (down_interruptible(&(dev->sem))!=0) {
		pr_err("%s: could not lock during open\n", MYDEV_NAME);
	} 

	tmp_buff = (unsigned char*) kzalloc(count, GFP_KERNEL);	
	if (tmp_buff == NULL) {
		pr_err("%s: failed due to out of memory operation", __func__);
		retval = -ENOMEM;
		goto brkpnt;
	}

	if (copy_from_user(tmp_buff, buf, count) != 0)
	{
		retval = -EFAULT;
		goto brkpnt;
	}

	for (f = 0; f < count; f++)
	{
		dev->ramdisk[*f_pos + f] = tmp_buff[f];
	}
	*f_pos += count;

	retval = count;
brkpnt:
	if (tmp_buff != NULL)
		kfree(tmp_buff);

	up(&(dev->sem));
	return retval;
}


loff_t mycdev_llseek(struct file *filp, loff_t off, int arrow)
{
	struct ASP_mycdev *dev = (struct ASP_mycdev *)filp->private_data;
	loff_t new_pos = 0;       
	unsigned char* temp;
	if (down_interruptible(&(dev->sem))!=0) {
		pr_err("%s: could not lock during open\n", MYDEV_NAME);
	}

	switch(arrow) {
		case 0: /* SEEK_SET */
			new_pos = off;
			break;

		case 1: /* SEEK_CUR */
			new_pos = filp->f_pos + off;
			break;

		case 2: /* SEEK_END */
			new_pos = dev->buffer_size + off;
			break;
	}

	if (new_pos > dev->buffer_size)
	{
		// Here the buffer size needed is less than the buffer size alloted
		temp = (unsigned char*)kzalloc(new_pos,GFP_KERNEL);
		memcpy(temp, dev->ramdisk, dev->buffer_size);
		kfree(dev->ramdisk);
		dev->ramdisk = temp;
		dev->buffer_size = new_pos;
	}

	filp->f_pos = new_pos;

	up(&(dev->sem));
	return new_pos;
}

int mycdev_release(struct inode *inode, struct file *filp)
{
        return 0;
}

struct file_operations mycdev_fops = {
        .owner =    THIS_MODULE,
        .read =     mycdev_read,
        .write =    mycdev_write,
        .open =     mycdev_open,
        .release =  mycdev_release,
        .llseek =   mycdev_llseek,
	.unlocked_ioctl = mycdev_ioctl,
};

static int mycdev_create_device(struct ASP_mycdev *dev, int minor,struct class *class)
{
        dev_t devno = MKDEV(majornum, minor);
        struct device *device = NULL;

        dev->buffer_size = ramdisk_size;
	dev->devNo = minor;
	dev->ramdisk = NULL; 
	sema_init(&(dev->sem),1);
        
        cdev_init(&dev->cdev, &mycdev_fops);
        dev->cdev.owner = THIS_MODULE;

	// take the memory for ramdisk in the structure ASP_mycdev
        dev->ramdisk = (unsigned char*)kzalloc(dev->buffer_size, GFP_KERNEL);

        cdev_add(&dev->cdev, devno, 1);

        device = device_create(class, NULL, devno, NULL,MYDEV_NAME "%d", minor);

        return 0;
}

static void mycdev_cleanup(void)
{
        int f = 0;
        struct list_head *curr = NULL;
        struct ASP_mycdev *dev = NULL;

doagain:
	list_for_each(curr, &dev_head) 
	{
		dev = list_entry(curr, struct ASP_mycdev, list);
		device_destroy(dev_group, MKDEV(majornum, f));
        	cdev_del(&dev->cdev);
        	kfree(dev->ramdisk);
		list_del(&(dev->list));
                kfree(dev);
		f++;
        	goto doagain;
        }

	if (dev_group)
		class_destroy(dev_group);

	unregister_chrdev_region(MKDEV(majornum, 0), NUM_DEVICES);
	return;
}

static int __init my_init(void)
{	
	int f;
        dev_t dev = 0;
        struct ASP_mycdev *char_dev = NULL;

	// Register the devices for the user level applications to use it
        alloc_chrdev_region(&dev, 0, NUM_DEVICES, MYDEV_NAME);
        
        majornum = MAJOR(dev);
        dev_group = class_create(THIS_MODULE, MYDEV_NAME);
         
	for (f = 0; f < NUM_DEVICES; ++f) 
        {
		char_dev = (struct ASP_mycdev *)kzalloc(sizeof(struct ASP_mycdev),GFP_KERNEL);
		mycdev_create_device(char_dev, f, dev_group);

		INIT_LIST_HEAD(&(char_dev->list));
		list_add(&(char_dev->list), &dev_head);
        }

        return 0;
}

static void __exit my_exit(void)
{
	// Unregister the devices and take back all the relevant resorces alloted to it
	// Do it in the reverse order of registeration
        mycdev_cleanup();
        return;
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Shreyas Sreedhara");
MODULE_LICENSE("GPL v2");

