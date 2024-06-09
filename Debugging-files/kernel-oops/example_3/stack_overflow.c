/* stack_overflow.c */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static void recursive_function(int i) {
    if (i > 0) {
        recursive_function(i - 1);
    }
}

static int __init stack_overflow_init(void) {
    printk(KERN_INFO "Stack overflow module loaded\n");
    recursive_function(100000);  /* Large enough to cause a stack overflow */
    return 0;
}

static void __exit stack_overflow_exit(void) {
    printk(KERN_INFO "Stack overflow module unloaded\n");
}

module_init(stack_overflow_init);
module_exit(stack_overflow_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A module that causes stack overflow");
MODULE_AUTHOR("Example");
