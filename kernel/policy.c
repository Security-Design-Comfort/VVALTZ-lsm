#include <linux/kernel.h>
#include <linux/string.h>
#include "vvaltz.h"

#define MAX_RULES 64

struct vault_rule {
    char path[MAX_PATH_LEN];
    int active;
};

struct vault_rule policy_rules[MAX_RULES];

// Scans the active matrix to see if a system path is locked
int is_path_protected(const char *path) {
    size_t i;

    for (i = 0; i < MAX_RULES; i = i + 1) {
        if (policy_rules[i].active == 1) {
            if (strnstr(path, policy_rules[i].path, MAX_PATH_LEN) != NULL) {
                return 1;
            }
        }
    }
    return 0;
}

// Registers a new target directory into the security matrix
void add_protected_path(const char *path) {
    size_t i;
    int allocated = 0;

    for (i = 0; i < MAX_RULES; i = i + 1) {
        if (policy_rules[i].active == 1) {
            if (strncmp(policy_rules[i].path, path, MAX_PATH_LEN) == 0) {
                return;
            }
        }
    }

    for (i = 0; i < MAX_RULES; i = i + 1) {
        if (allocated == 0) {
            if (policy_rules[i].active == 0) {
                strncpy(policy_rules[i].path, path, MAX_PATH_LEN);
                policy_rules[i].active = 1;
                allocated = 1;
                
                printk(KERN_INFO "[VVALTZ] Protection rule armed at slot %zu for: %s\n", i, path);
            }
        }
    }
}

// Clears a path out of the matrix slot allocations
void remove_protected_path(const char *path) {
    size_t i;

    for (i = 0; i < MAX_RULES; i = i + 1) {
        if (policy_rules[i].active == 1) {
            if (strncmp(policy_rules[i].path, path, MAX_PATH_LEN) == 0) {
                policy_rules[i].active = 0;
                memset(policy_rules[i].path, 0, MAX_PATH_LEN);
                
                printk(KERN_INFO "[VVALTZ] Protection rule unmounted at slot %zu for: %s\n", i, path);
            }
        }
    }
}