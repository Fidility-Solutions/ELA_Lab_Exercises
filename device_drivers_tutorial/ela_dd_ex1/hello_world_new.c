/***************************************************************************//**
*  \file       hello_world.c
*
*  \details    Simple hello world driver
*
*  \author     Fidility SOlutions
*
*  \Reference  EmbeTronicX
* *******************************************************************************/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
 
/*
** Module Init function
*/
static int __init hello_world_init(void)
{
    pr_info("Welcome to Embedded Linux \n");
    pr_info("This is an example for simple module\n");
    pr_info("Kernel Module Inserted Successfully...\n");
    return 0;
}

/*
** Module Exit function
*/
static void __exit hello_world_exit(void)
{
    pr_info("Kernel Module Removed Successfully...\n");
}
 
module_init(hello_world_init);
module_exit(hello_world_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Fidility Solutions <info@fidilitysolutions.com>");
MODULE_DESCRIPTION("Your first hello world driver");
MODULE_VERSION("1.0");
