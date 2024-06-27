#include <linux/module.h>
#include <linux/kernel.h>

static int __init my_panic_init(void) {
    printk(KERN_INFO "Kernel panic test module loaded\n");
    panic("Forced kernel panic");
    return 0;
}

static void __exit my_panic_exit(void) {
    printk(KERN_INFO "Kernel panic test module unloaded\n");
}
module_init(my_panic_init);
module_exit(my_panic_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Test Author");
MODULE_DESCRIPTION("Kernel module to induce a kernel panic");

