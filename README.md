<div align="center">
  <img src="./image/logo.png" width="100">
</div>
<h1 align="center">VVALTZ Layer Engine</h1>
<h3 align="center">A lightweight linux security module that provides low-level folder protection by deploying dynamic hooks at Ring 0 to stop unauthorized access, even from root users.</h3>

## Installation
Git clone this repo and build your own module with Makefile
```bash
  git clone https://github.com/Security-Design-Comfort/VVALTZ-lsm.git
  cd /VVALTZ-lsm/kernel
  make
```

Install the module in the same folder
```bash
  sudo insmod vvaltz.ko
```

## Usages
Navigate to /VVALTZ/user to make cli file
```bash
  cd /VVALTZ-lsm/user
  g++ -O2 -Wall main.cpp -o vvaltz
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
## Notices
This is the result of development over in a short period of time.
AI is used to fix codes and check spelling.
Errors and bugs are expected as I am a newbie in this field.

Additionally, there is currently no safe way to remove this module.
If you want to uninstall this, just reboot your system and the module will be wiped out.
