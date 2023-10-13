/***************************************************************************//**
*  \file       ldd_module_param.c
*
*  \details    Simple driver with module parameters
*
*  \author     Fidility Solutions
*
*  \Reference  EmbeTronicX
* *******************************************************************************/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

/* Variable declarartion */
int s16Value, as16value[4];
char *psName;
int s16ValueNotify = 0;


/* macro to initialize the Variables, set permissions and create sysfs entry under module/parameters*/
module_param(s16Value, int, S_IRUSR|S_IWUSR);                     //integer value
module_param(psName, charp, S_IRUSR|S_IWUSR);                    //String
module_param_array(as16value, int, NULL, S_IRUSR|S_IWUSR);      //Array of integers
 
/*---- Module_param_cb()--------------------------------*/
int notify_param(const char *val, const struct kernel_param *kp)
{
        int res = param_set_int(val, kp); // Use helper for write variable
        if(res==0) {
                pr_info("Set Call back function called...\n");
                pr_info("New value of s16ValueNotify = %d\n", s16ValueNotify);
                return 0;
        }
        return -1;
}

const struct kernel_param_ops callback_param_ops = 
{
        .set = &notify_param, // Set function  ...
        .get = &param_get_int, // Get function ...
};

/* Module parameter with a callback */
module_param_cb(s16ValueNotify, &callback_param_ops, &s16ValueNotify, S_IRUGO|S_IWUSR );

/*-------------------------------------------------------------------------*/

/*
** Module init function
*/
static int __init module_param_init(void)
{
        int i;
        pr_info("Initial value of s16Value = %d  \n", s16Value);
        pr_info("Initial value of s16ValueNotify = %d  \n", s16ValueNotify);
        pr_info("Initial value of psName = %s \n", psName);
        for (i = 0; i < (sizeof as16value / sizeof (int)); i++) {
                pr_info("Initial value of as16value[%d] = %d\n", i, as16value[i]);
        }
        pr_info("Kernel Module Inserted Successfully...\n");
    return 0;
}

/*
** Module Exit function
*/
static void __exit module_param_exit(void)
{
    pr_info("Kernel Module Removed Successfully...\n");
}
 
module_init(module_param_init);
module_exit(module_param_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Fidility Solutions <info@fidilitysolutions.com>");
MODULE_DESCRIPTION("Simple driver for module parameters");
MODULE_VERSION("1.0");
