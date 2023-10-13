/****************************************************************************
*  \file       ldd_dynamic_major_minor.c
*
*  \details    Creating Major and Minor Numbers Dynamically
*
*  \author     Fidility Solutions
*
*  \Reference  EmbeTronicX
* *******************************************************************************/

#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
 
dev_t dev = 0;

/*
** Module Init function
*/
static int __init module_dynamic_init(void)
{
        /*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "Dynamic_Dev")) <0){
                pr_info("Cannot allocate major number for device 1\n");
                return -1;
        }
        pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
        pr_info("Kernel Module Inserted Successfully...\n");
        
        return 0;
}

/*
** Module exit function
*/
static void __exit module_dynamic_exit(void)
{
        unregister_chrdev_region(dev, 1);
        pr_info("Kernel Module Removed Successfully...\n");
}
 
module_init(module_dynamic_init);
module_exit(module_dynamic_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Fidility Solutions <info@fidilitysolutions.com>");
MODULE_DESCRIPTION("Simple linux driver (Dynamically allocating the Major and Minor number)");
MODULE_VERSION("1.0");
