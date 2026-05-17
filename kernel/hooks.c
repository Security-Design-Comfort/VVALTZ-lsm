#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/ptrace.h>
#include <linux/version.h>
#include <linux/uaccess.h>
#include <linux/sched/signal.h>
#include <asm/ptrace.h>
#include "vvaltz.h"

static struct kprobe openat_kp;
static struct kprobe openat2_kp;
static struct kprobe rename_kp;

static void overkill_terminate(const char *path, const char *syscall_name) {
    printk(KERN_ALERT "[VVALTZ] OVERKILL ENGAGED: Instantly terminating PID %d (%s) for attempting to access protected path via %s: %s\n",
           current->pid, current->comm, syscall_name, path);
    
    send_sig(SIGKILL, current, 0);
}

static int handler_openat_pre(struct kprobe *p, struct pt_regs *regs) {
    struct pt_regs *user_regs = (struct pt_regs *)regs->di;
    const char __user *filename;
    char k_filename[MAX_PATH_LEN];
    long copied;

    if (!user_regs) {
        return 0;
    }

    filename = (const char __user *)user_regs->si;

    if (filename != NULL) {
        copied = strncpy_from_user(k_filename, filename, sizeof(k_filename));
        if (copied > 0 && copied < MAX_PATH_LEN) {
            if (is_path_protected(k_filename) == 1) {
                overkill_terminate(k_filename, "openat");
                regs->ax = -EACCES;
                return 1;
            }
        }
    }
    return 0;
}

static int handler_openat2_pre(struct kprobe *p, struct pt_regs *regs) {
    struct pt_regs *user_regs = (struct pt_regs *)regs->di;
    const char __user *filename;
    char k_filename[MAX_PATH_LEN];
    long copied;

    if (!user_regs) {
        return 0;
    }

    filename = (const char __user *)user_regs->si;

    if (filename != NULL) {
        copied = strncpy_from_user(k_filename, filename, sizeof(k_filename));
        if (copied > 0 && copied < MAX_PATH_LEN) {
            if (is_path_protected(k_filename) == 1) {
                if (strnstr(k_filename, ".directory", MAX_PATH_LEN) || 
                    strnstr(k_filename, ".editorconfig", MAX_PATH_LEN)) {
                    return 0; 
                }

                overkill_terminate(k_filename, "openat2");
                regs->ax = -EACCES;
                return 1;
            }
        }
    }
    return 0;
}

static int handler_rename_pre(struct kprobe *p, struct pt_regs *regs) {
    struct pt_regs *user_regs = (struct pt_regs *)regs->di;
    const char __user *newpath;
    char k_newpath[MAX_PATH_LEN];
    long copied;

    if (!user_regs) {
        return 0;
    }

    newpath = (const char __user *)user_regs->cx;

    if (newpath != NULL) {
        copied = strncpy_from_user(k_newpath, newpath, sizeof(k_newpath));
        if (copied > 0 && copied < MAX_PATH_LEN) {
            if (is_path_protected(k_newpath) == 1) {
                overkill_terminate(k_newpath, "renameat2");
                regs->ax = -EACCES;
                return 1;
            }
        }
    }
    return 0;
}

int install_hooks(void) {
    int ret;

    openat_kp.symbol_name = "__x64_sys_openat"; 
    openat_kp.pre_handler = handler_openat_pre;
    ret = register_kprobe(&openat_kp);
    if (ret < 0) {
        printk(KERN_ERR "[VVALTZ] Failed to mount openat syscall wrapper\n");
        return ret;
    }

    openat2_kp.symbol_name = "__x64_sys_openat2";
    openat2_kp.pre_handler = handler_openat2_pre;
    ret = register_kprobe(&openat2_kp);
    if (ret < 0) {
        unregister_kprobe(&openat_kp);
        printk(KERN_ERR "[VVALTZ] Failed to mount openat2 syscall wrapper\n");
        return ret;
    }

    rename_kp.symbol_name = "__x64_sys_renameat2";
    rename_kp.pre_handler = handler_rename_pre;
    ret = register_kprobe(&rename_kp);
    if (ret < 0) {
        unregister_kprobe(&openat_kp);
        unregister_kprobe(&openat2_kp);
        printk(KERN_ERR "[VVALTZ] Failed to mount renameat2 syscall wrapper\n");
        return ret;
    }

    printk(KERN_INFO "[VVALTZ] Absolute Overkill Protection Engine Armed and Online.\n");
    return 0;
}

void remove_hooks(void) {
    unregister_kprobe(&openat_kp);
    unregister_kprobe(&openat2_kp);
    unregister_kprobe(&rename_kp);
    printk(KERN_INFO "[VVALTZ] File interception layers unmounted safely.\n");
}