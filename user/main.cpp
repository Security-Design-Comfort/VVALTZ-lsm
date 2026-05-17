#include <iostream>
#include <string>
#include <cstdlib>
#include "cli.h"

void print_usage() {
    std::cout << "=== VVALTZ SYSTEM UTILITY ===\n";
    std::cout << "Core Directives:\n";
    std::cout << "  vvaltz --add </path/to/dir>    : Secure a target directory node\n";
    std::cout << "  vvaltz --remove </path/to/dir> : Release a target directory node\n";
    std::cout << "  vvaltz --status                : Query current kernel storage matrix\n";
    std::cout << "-------------------------------------------------------\n";
    std::cout << "Preset Management (Recommended optional configuration templates):\n";
    std::cout << "  vvaltz --preset-list           : Display available security templates\n";
    std::cout << "  vvaltz --preset-apply <name>   : Safely deploy a specific template\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    std::string command = argv[1];

    if (command == "--status") {
        if (print_vault_status() == false) {
            std::cerr << "Runtime Error: Control node unreachable. Verify module status.\n";
            return 1;
        }
    } 
    else if (command == "--add" && argc == 3) {
        if (add_directory_to_vault(argv[2]) == false) {
            std::cerr << "Configuration Error: Failed to write to runtime storage matrix.\n";
            return 1;
        }
        std::cout << "Dispatched enforcement lock request for target: " << argv[2] << "\n";
    } 
    else if (command == "--remove" && argc == 3) {
        if (remove_directory_from_vault(argv[2]) == false) {
            std::cerr << "Configuration Error: Failed to purge from runtime storage matrix.\n";
            return 1;
        }
        std::cout << "Dispatched enforcement release request for target: " << argv[2] << "\n";
    } 
    // DIRECTIVE 1: LIST RECOMMENDED SECURITY PRESETS
    else if (command == "--preset-list") {
        std::cout << "\n--- AVAILABLE RECOMMENDED SECURITY PRESETS ---\n\n";
        size_t i;
        for (i = 0; i < SYSTEM_PRESETS.size(); i = i + 1) {
            std::cout << " [*] Preset Name: " << SYSTEM_PRESETS[i].name << "\n";
            std::cout << "     Description: " << SYSTEM_PRESETS[i].description << "\n";
            std::cout << "     Target Paths: ";
            size_t j;
            for (j = 0; j < SYSTEM_PRESETS[i].paths.size(); j = j + 1) {
                std::cout << SYSTEM_PRESETS[i].paths[j] << "  ";
            }
        }
    }
        
    // DIRECTIVE 2: DEPLOY SELECTED SECURITY PRESET
    else if (command == "--preset-apply" && argc == 3) {
        std::string selected_preset = argv[2];
        const char* user_env = std::getenv("USER");
        std::string username = "root";
        
        if (user_env != NULL) {
            username = std::string(user_env);
        }

        bool preset_found = false;
        size_t i;

        for (i = 0; i < SYSTEM_PRESETS.size(); i = i + 1) {
            if (SYSTEM_PRESETS[i].name == selected_preset) {
                preset_found = true;
                std::cout << "Deploying optional security preset template: [" << selected_preset << "]\n";
                
                size_t j;
                for (j = 0; j < SYSTEM_PRESETS[i].paths.size(); j = j + 1) {
                    std::string absolute_target;
                    
                    /* Dynamic absolute path construction resolved by context evaluation */
                    if (SYSTEM_PRESETS[i].paths[j].rfind("/.", 0) == 0 || 
                        SYSTEM_PRESETS[i].paths[j].find("/Documents/") != std::string::npos ||
                        SYSTEM_PRESETS[i].paths[j].find("/Downloads/") != std::string::npos) {
                        absolute_target = "/home/" + username + SYSTEM_PRESETS[i].paths[j];
                    } else {
                        absolute_target = SYSTEM_PRESETS[i].paths[j];
                    }

                    if (add_directory_to_vault(absolute_target) == true) {
                        std::cout << "  -> Secure protection armed for path: " << absolute_target << "\n";
                    } else {
                        std::cerr << "  [!] System write failure encountered for path: " << absolute_target << "\n";
                    }
                }
            }
        }

        if (preset_found == false) {
            std::cerr << "Deployment Error: Preset template '" << selected_preset << "' not found. Use --preset-list to verify.\n";
            return 1;
        }
    }
    else {
        print_usage();
        return 1;
    }

    return 0;
}
