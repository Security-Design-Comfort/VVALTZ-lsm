#ifndef CLI_H
#define CLI_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

const std::string PROC_VAULT_PATH = "/proc/vvaltz_control";

struct VaultPreset {
  std::string name;
  std::string description;
  std::vector<std::string> paths;
};

const std::vector<VaultPreset> SYSTEM_PRESETS = {
    {"ssh-keys",
     "Secures personal cryptographic SSH keys and configuration files",
     {"/.ssh/"}},

    {"browser-data",
     "Secures sensitive web browser profiles, cookies, and local session caches",
     {"/.config/google-chrome/", "/.mozilla/firefox/"}},

    {"user-documents",
     "Secures private daily document directories and down-stream system files",
     {"/Documents/", "/Downloads/"}}};

inline bool add_directory_to_vault(const std::string &path)
{
  std::ofstream proc_file(PROC_VAULT_PATH);
  if (proc_file.is_open() == false) {
    return false;
  }
  proc_file << "+ " << path << "\n";
  proc_file.close();
  return true;
}

inline bool remove_directory_from_vault(const std::string &path)
{
  std::ofstream proc_file(PROC_VAULT_PATH);
  if (proc_file.is_open() == false) {
    return false;
  }
  proc_file << "- " << path << "\n";
  proc_file.close();
  return true;
}

inline bool restore_module_visibility()
{
  std::ofstream proc_file(PROC_VAULT_PATH);
  if (proc_file.is_open() == false) {
    return false;
  }
  proc_file << "U\n";
  proc_file.close();
  return true;
}

inline bool hide_module_visibility()
{
  std::ofstream proc_file(PROC_VAULT_PATH);
  if (proc_file.is_open() == false) {
    return false;
  }
  proc_file << "H\n";
  proc_file.close();
  return true;
}

inline bool print_vault_status()
{
  std::ifstream proc_file(PROC_VAULT_PATH);
  if (proc_file.is_open() == false) {
    return false;
  }
  std::string line;
  while (std::getline(proc_file, line)) {
    std::cout << line << "\n";
  }
  proc_file.close();
  return true;
}

#endif // CLI_H