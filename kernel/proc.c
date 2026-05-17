#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/slab.h>
#include "vvaltz.h"

#define PROCFS_NAME "vvaltz_control"
static struct proc_dir_entry *vvaltz_proc_file = NULL;

#define MAX_RULES 64
struct vault_rule {
    char path[MAX_PATH_LEN];
    int active;
};
extern struct vault_rule policy_rules[MAX_RULES];


static ssize_t vvaltz_control_write(struct file *file, const char __user *ubuf, size_t count, loff_t *ppos) {
    char kbuf[MAX_PATH_LEN + 4];
    size_t len;
    char operation_type;
    char *target_path;

    if (count > MAX_PATH_LEN + 3) {
        len = MAX_PATH_LEN + 3;
    } else {
        len = count;
    }

    if (copy_from_user(kbuf, ubuf, len)) {
        return -EFAULT;
    }

    kbuf[len] = '\0';

    if (len < 3) {
        return -EINVAL;
    }

    operation_type = kbuf[0];
    target_path = kbuf + 2;

    len = strlen(target_path);
    if (len > 0) {
        if (target_path[len - 1] == '\n') {
            target_path[len - 1] = '\0';
        }
    }

    if (operation_type == '+') {
        add_protected_path(target_path);
    } 
    else if (operation_type == '-') {
        remove_protected_path(target_path);
    } 
    else {
        return -EINVAL;
    }

    return count;
}

static ssize_t vvaltz_control_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos) {
    char *page_buffer;
    ssize_t len = 0;
    size_t i;
    int items_found = 0;

    page_buffer = kmalloc(4096, GFP_KERNEL);
    if (page_buffer == NULL) {
        return -ENOMEM;
    }

    len = len + snprintf(page_buffer + len, 4096 - len, "=== VVALTZ ACTIVE CORE ===\n");

    for (i = 0; i < MAX_RULES; i = i + 1) {
        if (policy_rules[i].active == 1) {
            len = len + snprintf(page_buffer + len, 4096 - len, "Protected: %s\n", policy_rules[i].path);
            items_found = items_found + 1;
        }
    }

    if (items_found == 0) {
        len = len + snprintf(page_buffer + len, 4096 - len, "No paths are currently protected.\n");
    }

    if (*ppos >= len) {
        kfree(page_buffer);
        return 0;
    }

    if (count < (len - *ppos)) {
        kfree(page_buffer);
        return -EINVAL;
    }

    len = len - *ppos;

    if (copy_to_user(ubuf, page_buffer + *ppos, len)) {
        kfree(page_buffer);
        return -EFAULT;
    }

    *ppos = *ppos + len;
    kfree(page_buffer);
    return len;
}

static const struct proc_ops vvaltz_proc_fops = {
    .proc_read  = vvaltz_control_read,
    .proc_write = vvaltz_control_write,
};

int init_proc_interface(void) {
    vvaltz_proc_file = proc_create(PROCFS_NAME, 0666, NULL, &vvaltz_proc_fops);
    if (vvaltz_proc_file == NULL) {
        proc_remove(vvaltz_proc_file);
        printk(KERN_ERR "[VVALTZ] Critical Error: Unable to instantiate proc control interface node.\n");
        return -ENOMEM;
    }
    printk(KERN_INFO "[VVALTZ] Control boundary mapped successfully at /proc/%s\n", PROCFS_NAME);
    return 0;
}

void remove_proc_interface(void) {
    if (vvaltz_proc_file != NULL) {
        proc_remove(vvaltz_proc_file);
    }
    printk(KERN_INFO "[VVALTZ] Control boundary unmapped safely from procfs.\n");
}