#include <iostream>
#include <string>
#include <cstdlib>
#include "cli.h"

void print_usage() {
    std::cout << "=== VVALTZ SYSTEM UTILITY ===\n";
    std::cout << "  --add </path/to/dir>    : Arm and protect a specific path\n";
    std::cout << "  --remove </path/to/dir> : Release dynamic lock from a path\n";
    std::cout << "  --status                : Inspect current driver policy matrix\n";
    std::cout << "  --preset-list           : List all available system templates\n";
    std::cout << "  --preset-apply <name>   : Deploy a target configuration template\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    std::string command = argv[1];

    if (command == "--status") {
        if (print_vault_status() == false) {
            std::cerr << "Runtime Error: Proc connection dropped. Verify module is loaded.\n";
            return 1;
        }
    } 
    else if (command == "--add" && argc == 3) {
        if (add_directory_to_vault(argv[2]) == false) {
            std::cerr << "Configuration Error: Failed writing target node path into engine.\n";
            return 1;
        }
        std::cout << "Dispatched protection request for target: " << argv[2] << "\n";
    } 
    else if (command == "--remove" && argc == 3) {
        if (remove_directory_from_vault(argv[2]) == false) {
            std::cerr << "Configuration Error: Failed purging target node path out of engine.\n";
            return 1;
        }
        std::cout << "Dispatched release request for target: " << argv[2] << "\n";
    } 
    // OPERATION 1: SHOWCASE PRESET TEMPLATES
    else if (command == "--preset-list") {
        std::cout << "\n=== SYSTEM RECOMMENDED PROFILES ===\n\n";
        size_t i;
        for (i = 0; i < SYSTEM_PRESETS.size(); i = i + 1) {
            std::cout << " [*] Template Name: " << SYSTEM_PRESETS[i].name << "\n";
            std::cout << "     Description  : " << SYSTEM_PRESETS[i].description << "\n";
            std::cout << "     Target Paths : ";
            size_t j;
            for (j = 0; j < SYSTEM_PRESETS[i].paths.size(); j = j + 1) {
                std::cout << SYSTEM_PRESETS[i].paths[j] << "  ";
            }
        }
    }
    // OPERATION 2: RESOLVE AND INJECT PRESET TARGETS
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
                std::cout << "Parsing recommended configuration template: [" << selected_preset << "]\n";
                
                size_t j;
                for (j = 0; j < SYSTEM_PRESETS[i].paths.size(); j = j + 1) {
                    std::string absolute_target;
                    
                    /* Dynamically assemble home folder paths for the current environment */
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
                        std::cerr << "  [!] Storage stream write error for path: " << absolute_target << "\n";
                    }
                }
            }
        }

        if (preset_found == false) {
            std::cerr << "Deployment Error: Template named '" << selected_preset << "' does not exist. Run --preset-list to view entries.\n";
            return 1;
        }
    }
    else {
        print_usage();
        return 1;
    }

    return 0;
}
