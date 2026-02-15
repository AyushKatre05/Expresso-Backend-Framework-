#include <iostream>
#include <cstring>

// This is the C-compatible bridge that Rust will call.
// It acts as the entry point to your C++ server logic.

extern "C" {
    int cpp_process_command(const char* command) {
        // This code runs inside the C++ Engine!
        std::cout << "[C++ CORE] Received Command via FFI: " << command << std::endl;
        
        // internal logic simulation
        if (strcmp(command, "hack") == 0) {
            std::cout << "[C++ CORE] SECURITY ALERT: Intrusion attempt detected!" << std::endl;
            return 999; // Special status code
        }
        
        return 1; // Success
    }
}
