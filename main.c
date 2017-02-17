#include <linux/module.h> // Core header for loading LKMs into the kernel
#include <linux/kernel.h> // Contains types, macros, functions for the kernel
#include <linux/version.h> // contains the currrent version of the linux
#include <linux/fs.h>      // Header for the Linux file system support
#include <linux/cdev.h>    // Header for character device 
#include <linux/device.h>   // Header to support the kernel Driver Model
#include <linux/errno.h>    //Header file for error number
#include <asm/uaccess.h>    // Required for the copy to user function
#include <linux/timex.h>    //Required for the timer regiser for timer ticks
#include <asm/msr.h>        //requied for mashine specific register
#include <linux/delay.h>    // reuired to produce delay

 
#include "query_ioctl.h"   // User defined header file
 
#define FIRST_MINOR 0
#define MINOR_CNT 1
 
static dev_t dev;        // the type used to r epresent device numbers within the kernel

static struct cdev c_dev; //one of the element of the inode structure
static cycles_t p,r;      // static variables of type cycles_t
static struct class *cl;   // declaring character class
static int lowerrange = 1, upperrange = 1000, random=0; //declaring & initializing three variables 
 /*Kernel  module to open characer device */
static int my_open(struct inode *i, struct file *f) //
{
    return 0;
}
/*Kernel  module to close characer device */
static int my_close(struct inode *i, struct file *f)
{
    return 0;
}
/*Kernel module to perform operation on the device*/
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
static int my_ioctl(struct inode *i, struct file *f, unsigned int cmd, unsigned long arg)
#else
static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
#endif
{ /*To find random number*/
	int i;
	query_arg_t q;	
   	 int getrandom(void){
	 p = get_cycles();
        for(i=0;i<jiffies;i++){}
	 msleep(jiffies%10000);
	   
	   r = get_cycles();
	 	random = abs(r-p); // assigns absolute value of the random number
	
	check1: if (abs(random) > upperrange)
		{
			random =abs(random)%upperrange;
		}
		
	while (abs(random) < lowerrange)
		{
			random = abs(random) + lowerrange;
 		}
		 if (abs(random) > upperrange){
		goto check1;
		}
		return 0;
	}
   switch (cmd)
    {    /*//passing variables to the user*/
        case QUERY_GET_VARIABLES:
	     i=	getrandom();
            q.lowerrange = lowerrange; 
            q.upperrange = upperrange;
	     q.random  = abs(random);
            
            if (copy_to_user((query_arg_t *)arg, &q, sizeof(query_arg_t)))
            {
                return -EACCES;
            }
            break;
       /*clearing variables if requested by user */
        case QUERY_CLR_VARIABLES:
            lowerrange = 0;
            upperrange = 0;
           
            break;
     /*give the value from user to the kernel*/
        case QUERY_SET_VARIABLES:
            if (copy_from_user(&q, (query_arg_t *)arg, sizeof(query_arg_t)))
            {
                return -EACCES;
            }
            lowerrange = q.lowerrange;
            upperrange = q.upperrange;
            
            break;
	case QUERY_READWRITE_VARIABLES:
		if (copy_from_user(&q, (query_arg_t *)arg, sizeof(query_arg_t)))
            {
                return -EACCES;
            }
            lowerrange = q.lowerrange;
            upperrange = q.upperrange;
	
		i =getrandom();
	    q.random  = abs(random);
		if (copy_to_user((query_arg_t *)arg, &q, sizeof(query_arg_t)))
            {
                return -EACCES;
            }
            break;
        default:
            return -EINVAL;
    }
 
    return 0;
}
 /*structure to for converting user operation to the kernel opration*/
static struct file_operations query_fops =
{
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
    .ioctl = my_ioctl
#else
    .unlocked_ioctl = my_ioctl
#endif
};
 
static int __init query_ioctl_init(void)
{
    int ret;
    struct device *dev_ret;
 
 
    if ((ret = alloc_chrdev_region(&dev, FIRST_MINOR, MINOR_CNT, "query_ioctl")) < 0)
    {
        return ret;
    }
 
    cdev_init(&c_dev, &query_fops);
 
    if ((ret = cdev_add(&c_dev, dev, MINOR_CNT)) < 0)
    {
        return ret;
    }
     
    if (IS_ERR(cl = class_create(THIS_MODULE, "char")))
    {
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(cl);
    }
    if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, "trng_dev")))
    {
        class_destroy(cl);
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(dev_ret);
    }
 
    return 0;
}
 
static void __exit query_ioctl_exit(void)
{
    device_destroy(cl, dev);
    class_destroy(cl);
    cdev_del(&c_dev);
    unregister_chrdev_region(dev, MINOR_CNT);
}
 
module_init(query_ioctl_init);
module_exit(query_ioctl_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("nandan");
MODULE_DESCRIPTION("Query ioctl() Char Driver");

