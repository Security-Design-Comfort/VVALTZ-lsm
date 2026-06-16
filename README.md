<div align="center">
  <img src="./image/logo.png" width="100">
</div>
<h1 align="center">VVALTZ Layer Engine</h1>
<p align="center">A lightweight linux security module that provides low-level folder protection by deploying dynamic hooks at Ring 0 to stop unauthorized access, even from root users.</p>

## Installation
Git clone this repo and build your own module with Makefile
```bash
  git clone https://github.com/Security-Design-Comfort/VVALTZ-lsm.git
  cd VVALTZ-lsm/kernel
  make
```

Install the module in the same folder
```bash
  sudo insmod vvaltz.ko
```

## Usages
Navigate to /VVALTZ/user to make cli file
```bash
  cd VVALTZ-lsm/user
  g++ -o2 -Wall main.cpp -o vvaltz
```

Basic commands:
```bash
# View syntax parameters
./vvaltz

# Inspect current protected matrix paths active inside the module
./vvaltz --status

# Add a specific directory into the real-time locking matrix
./vvaltz --add /absolute/path/to/target

# Release a directory lock out of the security policy matrix
./vvaltz --remove /absolute/path/to/target

```

Deploying built-in recommended templates:
```bash
# List all built-in configuration templates
./vvaltz --preset-list

# Deploy a specific profile
./vvaltz --preset-apply ssh-keys
```

Advanced Module Control (Anti-Tampering)
```bash
# Hide the module from lsmod (default)
./vvaltz --hide

# Makes it visible to lsmod
./vvaltz --restore
```

## Uninstallation
Because VVALTZ implements advanced structural cloaking inside the kernel, you cannot directly run rmmod while the module is hidden.

To safely unload the module from memory without rebooting, follow this exact sequence:
```bash
# 1. Force the module to restore its links to the kernel module list
./vvaltz --restore

# 2. Unload the module from the system safely
sudo rmmod vvaltz
```

## Notices
- **Development Scope**: This project is the result of an intensive 8-week engineering sprint.
- **Tooling Disclosure**: AI assistance was carefully integrated during development exclusively for code optimizations, syntax refinement, and minor spelling corrections.
- **Disclaimer**: This software interacts directly with critical system-level abstractions inside Kernel Space. While it has been thoroughly stress-tested for stability under current configurations, unforeseen edge-case errors or bugs may occur. Use with caution in production environments.
