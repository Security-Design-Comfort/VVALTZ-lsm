#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/ptrace.h>
#include <linux/version.h>
#include <linux/uaccess.h>
#include <linux/sched/signal.h> 
#include <asm/ptrace.h>
#include "vvaltz.h"

static struct kprobe openat_kp;
static struct kprobe rename_kp;

// Overkill Enforcement: Instantly terminates the calling process
static void overkill_terminate(const char *path, const char *syscall_name)
{
    printk(KERN_ALERT "[VVALTZ] OVERKILL ENGAGED: Instantly terminating PID %d (%s) for attempting to access protected path via %s: %s\n",
           current->pid, current->comm, syscall_name, path);
    
    send_sig(SIGKILL, current, 0);
}

// Hook Handler for do_sys_openat2 (Intercepts baseline file openings)
static int handler_open_core_pre(struct kprobe *p, struct pt_regs *regs)
{
    // In x86_64 ABI, the second argument of do_sys_openat2 (filename) is passed via SI register
    const char __user *filename = (const char __user *)regs->si;
    char k_filename[MAX_PATH_LEN];
    long copied;

    if (filename != NULL) {
        copied = strncpy_from_user(k_filename, filename, sizeof(k_filename));
        if (copied > 0 && copied < MAX_PATH_LEN) {
            if (is_path_protected(k_filename) == 1) {
                // Safeguard desktop environments from crashing by bypassing config reads
                if (strnstr(k_filename, ".directory", MAX_PATH_LEN) != NULL || 
                    strnstr(k_filename, ".editorconfig", MAX_PATH_LEN) != NULL) {
                    return 0; 
                }

                overkill_terminate(k_filename, "open_core");
                
                // Override return value to trigger standard permission denied matrix
                regs->ax = -EACCES;
                return 1;
            }
        }
    }
    return 0;
}

// Hook Handler for do_renameat2 (Intercepts file modifications and displacement)
static int handler_rename_core_pre(struct kprobe *p, struct pt_regs *regs)
{
    const char __user *newpath = (const char __user *)regs->cx;
    char k_newpath[MAX_PATH_LEN];
    long copied;

    if (newpath != NULL) {
        copied = strncpy_from_user(k_newpath, newpath, sizeof(k_newpath));
        if (copied > 0 && copied < MAX_PATH_LEN) {
            if (is_path_protected(k_newpath) == 1) {
                overkill_terminate(k_newpath, "rename_core");
                regs->ax = -EACCES;
                return 1;
            }
        }
    }
    return 0;
}

int install_hooks(void)
{
    int ret;

    // Bind directly to kernel core routines to completely resolve the "Invalid Parameters" error 
    openat_kp.symbol_name = "do_sys_openat2"; 
    openat_kp.pre_handler = handler_open_core_pre;
    ret = register_kprobe(&openat_kp);
    if (ret < 0) {
        printk(KERN_ERR "[VVALTZ] Failed to mount dynamic open core interceptor\n");
        return ret;
    }

    rename_kp.symbol_name = "do_renameat2";
    rename_kp.pre_handler = handler_rename_core_pre;
    ret = register_kprobe(&rename_kp);
    if (ret < 0) {
        unregister_kprobe(&openat_kp);
        printk(KERN_ERR "[VVALTZ] Failed to mount dynamic rename core interceptor\n");
        return ret;
    }

    printk(KERN_INFO "[VVALTZ] System Interception Matrix Armed and Online via Core Kernel Symbols.\n");
    return 0;
}

void remove_hooks(void)
{
    unregister_kprobe(&openat_kp);
    unregister_kprobe(&rename_kp);
    printk(KERN_INFO "[VVALTZ] Interception layers unmounted safely.\n");
}
