#include <iostream>
#include <string>
#include "../includes/StreamManager.hpp"
#include "../includes/Colors.hpp"

/**
 * @brief Displays the usage instructions for the program
 * 
 * @param programName The name of the executable
 */
void displayUsage(const char* programName) {
    std::cerr << B RED "Usage: " RESET RED << programName 
              << B " [start|stop|status]" RESET << std::endl;
}

/**
 * @brief Main entry point of the program
 * 
 * Processes command line arguments to control the YouTube streaming service.
 * 
 * @param argc Number of arguments
 * @param argv Array of arguments
 * @return Exit code
 */
int main(int argc, char **argv) {
    try {
        if (argc != 2) {
            displayUsage(argv[0]);
            return 1;
        }
        
        std::string action = argv[1];
        StreamManager streamManager;
        
        if (action == "start") {
            streamManager.startStream();
            std::cout << B GREEN "Stream started successfully." RESET << std::endl;
        } else if (action == "stop") {
            streamManager.stopStream();
            std::cout << B GREEN "Stream stopped successfully." RESET << std::endl;
        } else if (action == "status") {
            bool isRunning = streamManager.getStreamStatus();
            if (isRunning) {
                std::cout << B GREEN "Stream is currently running." RESET << std::endl;
            } else {
                std::cout << B YELLOW "Stream is not running." RESET << std::endl;
            }
        } else {
            displayUsage(argv[0]);
            return 1;
        }
    } catch (const std::exception &e) {
        std::cerr << B RED "Error: " RESET RED << e.what() << RESET << std::endl;
        return 1;
    }
    
    return 0;
}
