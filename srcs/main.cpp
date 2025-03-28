#include <iostream>
#include <stdexcept>
#include "../includes/StreamManager.hpp"
#include "../includes/ConfigManager.hpp"
#include "../includes/Colors.hpp"

/**
 * @brief Displays the usage instructions for the program
 * 
 * @param programName The name of the executable
 */
void displayUsage(const char* programName) {
    std::cerr << B BLUE "PageStreamer - Stream web pages to platforms" RESET << std::endl;
    std::cerr << B CYAN "Usage: " RESET CYAN << programName 
              << B " [start|stop|status|--config [PLATFORM|STREAM_KEY|STREAM_URL|see]|--schedule]" RESET << std::endl;
    std::cerr << "Commands:" << std::endl;
    std::cerr << "  start         Start streaming" << std::endl;
    std::cerr << "  stop          Stop streaming" << std::endl;
    std::cerr << "  status        Check stream status" << std::endl;
    std::cerr << "  --config      Configure all stream settings" << std::endl;
    std::cerr << "  --config PLATFORM    Configure streaming platform" << std::endl;
    std::cerr << "  --config STREAM_KEY  Configure stream key" << std::endl;
    std::cerr << "  --config STREAM_URL  Configure website URL to stream" << std::endl;
    std::cerr << "  --config see         View current configuration" << std::endl;
    std::cerr << "  --schedule     Configure automatic scheduling" << std::endl;
}

/**
 * @brief Handles the scheduling setup using crontab
 * 
 * This function shows the path to the crontab.txt file and waits for
 * user confirmation to update the crontab.
 * 
 * @return bool True if scheduling was successful
 */
bool handleSchedule() {
    std::string homeDir = getenv("HOME");
    std::string crontabPath = homeDir + "/stream/scripts/cron/crontab.txt";
    std::string updateScriptPath = homeDir + "/stream/scripts/cron/update_crontab.sh";
    
    std::cout << B CYAN "Schedule Configuration" RESET << std::endl;
    std::cout << "You can edit the crontab file to customize your streaming schedule." << std::endl;
    std::cout << CYAN "Crontab file location: " RESET << crontabPath << std::endl;
    std::cout << std::endl;
    std::cout << "Default schedule:" << std::endl;
    std::cout << "- Start: Friday, Saturday, Sunday at 20:30 (Paris time)" << std::endl;
    std::cout << "- Stop: Saturday, Sunday, Monday at 03:30 (Paris time)" << std::endl;
    std::cout << std::endl;
    std::cout << YELLOW "Once you've edited the crontab file, type 'yes' to update" << std::endl;
    std::cout << "or 'no' to cancel: " RESET;
    
    std::string response;
    std::cin >> response;
    
    if (response == "yes" || response == "YES" || response == "Yes") {
        std::cout << "Updating crontab..." << std::endl;
        
        // Make sure the update script is executable
        std::string chmodCmd = "chmod +x \"" + updateScriptPath + "\"";
        system(chmodCmd.c_str());
        
        // Execute the update script
        std::string updateCmd = "bash \"" + updateScriptPath + "\"";
        int result = system(updateCmd.c_str());
        
        if (result == 0) {
            std::cout << GREEN "Schedule updated successfully!" RESET << std::endl;
            return true;
        } else {
            std::cout << RED "Failed to update schedule. Check permissions and try again." RESET << std::endl;
            return false;
        }
    } else {
        std::cout << "Schedule update cancelled." << std::endl;
        return false;
    }
}

/**
 * @brief Main entry point of the program
 * 
 * Processes command line arguments to control the streaming service.
 * 
 * @param argc Number of arguments
 * @param argv Array of arguments
 * @return Exit code
 */
int main(int argc, char **argv) {
    try {
        if (argc < 2) {
            displayUsage(argv[0]);
            return 1;
        }
        
        std::string action = argv[1];
        
        if (action == "--config") {
            ConfigManager configManager;
            if (argc > 2) {
                return configManager.handleConfig(argv[2]) ? 0 : 1;
            } else {
                return configManager.handleConfig("") ? 0 : 1;
            }
        } else if (action == "--schedule") {
            return handleSchedule() ? 0 : 1;
        }
        
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