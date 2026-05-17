#ifndef CLI_H
#define CLI_H

#include <string>
#include <fstream>
#include <iostream>

const std::string PROC_VAULT_PATH = "/proc/vvaltz_control";

inline bool add_directory_to_vault(const std::string& path) {
    std::ofstream proc_file(PROC_VAULT_PATH);
    if (proc_file.is_open() == false) {
        return false;
    }
    proc_file << "+ " << path << "\n";
    proc_file.close();
    return true;
}

inline bool remove_directory_from_vault(const std::string& path) {
    std::ofstream proc_file(PROC_VAULT_PATH);
    if (proc_file.is_open() == false) {
        return false;
    }
    proc_file << "- " << path << "\n";
    proc_file.close();
    return true;
}

inline bool print_vault_status() {
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

#endif