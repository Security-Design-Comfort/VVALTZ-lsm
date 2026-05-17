#ifndef VVALTZ_H
#define VVALTZ_H

#include <linux/types.h>

#define MAX_PATH_LEN 256

// Core Policy Declarations
int is_path_protected(const char *path);
void add_protected_path(const char *path);
void remove_protected_path(const char *path);

// Hook Lifecycle Management
int install_hooks(void);
void remove_hooks(void);

// Proc Interface Management
int init_proc_interface(void);
void remove_proc_interface(void);

#endif