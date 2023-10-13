/***************************************************************************//**
*  \file       ldd_device_file_manual.c
*
*  \details    Simple linux driver (Manually Creating a Device file)
*
*  \Reference  EmbeTronicX
*
*******************************************************************************/
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
 
dev_t dev = 0;

/*
** Module init function
*/
static int __init device_manual_init(void)
{
        /*Allocating Major number dynamically */
        if((alloc_chrdev_region(&dev, 0, 1, "Manual_Device")) <0){
                pr_err("Cannot allocate major number for device\n");
                return -1;
        }
        
        pr_info("Kernel Module Inserted Successfully...\n");
        return 0;
}

/*
** Module exit function
*/
static void __exit device_manual_exit(void)
{
        unregister_chrdev_region(dev, 1);
        pr_info("Kernel Module Removed Successfully...\n");
}
 
module_init(device_manual_init);
module_exit(device_manual_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Fidility Solutions <info@fidilitysolutions.com>");
MODULE_DESCRIPTION("Simple linux driver (Manually Creating a Device file)");
MODULE_VERSION("1.0");
