#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <limits>
#include <cstdlib>
#include "../includes/StreamManager.hpp"
#include "../includes/Colors.hpp"

/**
 * @brief Displays the usage instructions for the program
 * 
 * @param programName The name of the executable
 */
void displayUsage(const char* programName) {
    std::cerr << B BLUE "PageStreamer - Stream web pages to platforms" RESET << std::endl;
    std::cerr << B CYAN "Usage: " RESET CYAN << programName 
              << B " [start|stop|status|--config [PLATFORM|STREAM_KEY]]" RESET << std::endl;
    std::cerr << "Commands:" << std::endl;
    std::cerr << "  start         Start streaming" << std::endl;
    std::cerr << "  stop          Stop streaming" << std::endl;
    std::cerr << "  status        Check stream status" << std::endl;
    std::cerr << "  --config      Configure stream settings" << std::endl;
}

/**
 * @brief Updates a value in the .env file
 * 
 * This function reads the .env file, finds the specified key,
 * and updates its value. If the key doesn't exist, it adds it.
 * 
 * @param key The environment variable name to update
 * @param value The new value to set
 * @return bool True if successful, false otherwise
 */
bool updateEnvFile(const std::string& key, const std::string& value) {
    std::string envPath = std::string(getenv("HOME")) + "/.pagestreamer/.env";
    std::string tempPath = envPath + ".tmp";
    std::ifstream inFile(envPath.c_str());
    std::ofstream outFile(tempPath.c_str());
    bool keyFound = false;
    std::string line;
    
    if (!outFile.is_open()) {
        std::cerr << RED "Error: Could not open .env file for writing" RESET << std::endl;
        return false;
    }
    
    if (inFile.is_open()) {
        while (std::getline(inFile, line)) {
            if (line.find(key + "=") == 0) {
                outFile << key << "=" << value << std::endl;
                keyFound = true;
            } else {
                outFile << line << std::endl;
            }
        }
        inFile.close();
    }
    
    if (!keyFound) {
        outFile << key << "=" << value << std::endl;
    }
    
    outFile.close();
    
    if (rename(tempPath.c_str(), envPath.c_str()) != 0) {
        std::cerr << RED "Error: Could not update .env file" RESET << std::endl;
        return false;
    }
    
    return true;
}

/**
 * @brief Configures the streaming platform using an interactive menu
 * 
 * Displays a list of platforms and prompts the user to select one
 * 
 * @return bool True if configuration was successful
 */
bool configurePlatform() {
    // Define Platform structure outside the function for C++98 compatibility
    struct PlatformInfo {
        std::string name;
        std::string rtmpUrl;
    };
    
    // Initialize platforms array manually without initializer lists
    std::vector<PlatformInfo> platforms;
    platforms.push_back(PlatformInfo{"YouTube", "rtmp://a.rtmp.youtube.com/live2"});
    platforms.push_back(PlatformInfo{"Twitch", "rtmp://live.twitch.tv/app"});
    platforms.push_back(PlatformInfo{"Facebook", "rtmp://live-api-s.facebook.com:80/rtmp"});
    platforms.push_back(PlatformInfo{"Instagram", "rtmp://live-upload.instagram.com:80/rtmp"});
    platforms.push_back(PlatformInfo{"TikTok", "rtmp://rtmp-push.tiktok.com/live"});
    platforms.push_back(PlatformInfo{"LinkedIn", "rtmp://rtmp.linkedin.com/live"});
    platforms.push_back(PlatformInfo{"Custom", ""});
    
    size_t selection = 0;
    std::string rtmpUrl;
    
    std::cout << B CYAN "Select streaming platform:" RESET << std::endl;
    
    for (size_t i = 0; i < platforms.size(); i++) {
        std::cout << CYAN << (i + 1) << ". " << platforms[i].name << RESET << std::endl;
    }
    
    std::cout << YELLOW "Enter number (1-" << platforms.size() << "): " RESET;
    std::cin >> selection;
    
    if (std::cin.fail() || selection < 1 || selection > platforms.size()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << RED "Invalid selection. Using YouTube as default." RESET << std::endl;
        selection = 1;
    }
    
    rtmpUrl = platforms[selection - 1].rtmpUrl;
    
    if (selection == platforms.size()) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << YELLOW "Enter custom RTMP URL: " RESET;
        std::getline(std::cin, rtmpUrl);
    }
    
    std::cout << "Setting platform to: " << platforms[selection - 1].name << std::endl;
    return updateEnvFile("PLATFORM", rtmpUrl);
}

/**
 * @brief Configures the streaming key by prompting the user
 * 
 * @return bool True if configuration was successful
 */
bool configureStreamKey() {
    std::string streamKey;
    
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    std::cout << B CYAN "Stream Key Configuration" RESET << std::endl;
    std::cout << YELLOW "Enter your stream key: " RESET;
    std::getline(std::cin, streamKey);
    
    if (streamKey.empty()) {
        std::cout << RED "Stream key cannot be empty" RESET << std::endl;
        return false;
    }
    
    return updateEnvFile("STREAM_KEY", streamKey);
}

/**
 * @brief Handles the configuration of stream settings
 * 
 * @param configType The specific setting to configure (optional)
 * @return bool True if configuration was successful
 */
bool handleConfig(const std::string& configType = "") {
    bool success = true;
    
    if (configType.empty() || configType == "PLATFORM") {
        success = configurePlatform() && success;
    }
    
    if (configType.empty() || configType == "STREAM_KEY") {
        success = configureStreamKey() && success;
    }
    
    if (success) {
        std::cout << GREEN "Configuration saved successfully!" RESET << std::endl;
        std::cout << "You can change settings anytime with: pagestreamer --config" << std::endl;
    } else {
        std::cout << RED "Configuration failed" RESET << std::endl;
    }
    
    return success;
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
            if (argc > 2) {
                return handleConfig(argv[2]) ? 0 : 1;
            } else {
                return handleConfig() ? 0 : 1;
            }
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
