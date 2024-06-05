#include <linux/module.h>
#include <linux/kernel.h>

int init_module(void) {
    printk(KERN_INFO "Kernel panic test module loaded\n");
    panic("Forced kernel panic");
    return 0;
}

void cleanup_module(void) {
    printk(KERN_INFO "Kernel panic test module unloaded\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Test Author");
MODULE_DESCRIPTION("Kernel module to induce a kernel panic");

