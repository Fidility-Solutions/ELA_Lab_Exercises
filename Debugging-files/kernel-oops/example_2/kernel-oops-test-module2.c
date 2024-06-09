#include <linux/module.h>
#include <linux/init.h>

static int __init kernel_oops_init(void) {
    int *ptr = (int*)0xDEADBEEF;  /* Invalid memory access */
        *ptr = 42;

    if (ptr != NULL) {
        /* If ptr is not NULL, print the value it points to */
        printk(KERN_INFO "Kernel Oops Module: %d\n", *ptr);
    } else {
        /* If ptr is NULL, print an error message */
        printk(KERN_ERR "Kernel Oops Module: Attempted to dereference a NULL pointer\n");
        /* Optionally, return an error code to indicate failure */
        return -EFAULT;
    }
   /* printk(KERN_INFO "Kernel Oops Module: %d\n", *ptr); */
    return 0;
}

static void __exit kernel_oops_exit(void) {
    printk(KERN_INFO "Kernel Oops Module exiting\n");
}

module_init(kernel_oops_init);
module_exit(kernel_oops_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");

