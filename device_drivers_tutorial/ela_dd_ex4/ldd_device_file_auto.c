/***************************************************************************//**
*  \file       ldd_device_file_auto.c
*
*  \details    Simple linux driver (Automatically Creating a Device file)
*
*  \Reference  EmbeTronicX
*
*******************************************************************************/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/device.h>
#include <linux/err.h>

dev_t dev = 0;
static struct class *dev_class;
 
/*
** Module init function
*/
static int __init device_auto_init(void)
{
        /*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "ext_dev")) <0){
                pr_err("Cannot allocate major number for device\n");
                return -1;
        }
        pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
        /*Creating struct class*/
	#ifdef CONFIG_ARM64
        dev_class = class_create(THIS_MODULE, "ext_class");
        #else
        dev_class = class_create("ext_class");
        #endif

        if(IS_ERR(dev_class)){
            pr_err("Cannot create the struct class for device\n");
            goto r_class;
        }
 
        /*Creating device*/
        if(IS_ERR(device_create(dev_class,NULL,dev,NULL,"ext_dev"))){
            pr_err("Cannot create the Device\n");
            goto r_device;
        }
        pr_info("Kernel Module Inserted Successfully...\n");
        return 0;
 
r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(dev,1);
        return -1;
}
 
/*
** Module exit function
*/
static void __exit device_auto_exit(void)
{
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
        unregister_chrdev_region(dev, 1);
        pr_info("Kernel Module Removed Successfully...\n");
}
 
module_init(device_auto_init);
module_exit(device_auto_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Fidility Solutions <info@fidilitysolutions.com>");
MODULE_DESCRIPTION("Simple linux driver (Automatically Creating a Device file)");
MODULE_VERSION("1.0");
