#include <iostream>
#include <string>
#include "cli.h"

void print_usage() {
    std::cout << "=== VVALTZ SYSTEM UTILITY ===\n";
    std::cout << "Available directives:\n";
    std::cout << "  vvaltz --add </path/to/dir>    : Secure directory node\n";
    std::cout << "  vvaltz --remove </path/to/dir> : Release directory node\n";
    std::cout << "  vvaltz --status                : Query current data matrix\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    std::string command = argv[1];

    if (command == "--status") {
        if (print_vault_status() == false) {
            std::cerr << "Runtime Error: Interface unreachable. Check driver installation.\n";
            return 1;
        }
    } 
    else if (command == "--add" && argc == 3) {
        if (add_directory_to_vault(argv[2]) == false) {
            std::cerr << "Configuration Error: Aborted writing to runtime storage matrix.\n";
            return 1;
        }
        std::cout << "Dispatched lock request for target: " << argv[2] << "\n";
    } 
    else if (command == "--remove" && argc == 3) {
        if (remove_directory_from_vault(argv[2]) == false) {
            std::cerr << "Configuration Error: Aborted writing to runtime storage matrix.\n";
            return 1;
        }
        std::cout << "Dispatched unlock request for target: " << argv[2] << "\n";
    } 
    else {
        print_usage();
        return 1;
    }

    return 0;
}