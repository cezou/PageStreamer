#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <limits>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <termios.h>
#include "../includes/StreamManager.hpp"
#include "../includes/Colors.hpp"

// Function prototypes 
bool configurePlatform();
bool configureStreamKey(bool inConfigSequence = false);
bool configureStreamUrl();
bool handleConfig(const std::string& configType = "");
bool showConfiguration();
std::string readPassword();

/**
 * @brief Displays the usage instructions for the program
 * 
 * @param programName The name of the executable
 */
void displayUsage(const char* programName) {
    std::cerr << B BLUE "PageStreamer - Stream web pages to platforms" RESET << std::endl;
    std::cerr << B CYAN "Usage: " RESET CYAN << programName 
              << B " [start|stop|status|--config [PLATFORM|STREAM_KEY|STREAM_URL|see]]" RESET << std::endl;
    std::cerr << "Commands:" << std::endl;
    std::cerr << "  start         Start streaming" << std::endl;
    std::cerr << "  stop          Stop streaming" << std::endl;
    std::cerr << "  status        Check stream status" << std::endl;
    std::cerr << "  --config      Configure all stream settings" << std::endl;
    std::cerr << "  --config PLATFORM    Configure streaming platform" << std::endl;
    std::cerr << "  --config STREAM_KEY  Configure stream key" << std::endl;
    std::cerr << "  --config STREAM_URL  Configure website URL to stream" << std::endl;
    std::cerr << "  --config see         View current configuration" << std::endl;
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
    
    // Make sure directory exists
    std::string dirPath = std::string(getenv("HOME")) + "/.pagestreamer";
    std::string mkdirCmd = "mkdir -p \"" + dirPath + "\"";
    system(mkdirCmd.c_str());
    
    // Set proper permissions
    std::string chmodCmd = "chmod 755 \"" + dirPath + "\"";
    system(chmodCmd.c_str());
    
    // Create empty .env file if it doesn't exist
    if (access(envPath.c_str(), F_OK) != 0) {
        std::ofstream createEnv(envPath.c_str());
        if (!createEnv.is_open()) {
            std::cerr << RED "Error: Could not create .env file. Check permissions for " 
                     << dirPath << RESET << std::endl;
            std::cerr << YELLOW "Try running: mkdir -p ~/.pagestreamer && touch ~/.pagestreamer/.env "
                     << "&& chmod 644 ~/.pagestreamer/.env" RESET << std::endl;
            return false;
        }
        createEnv.close();
        
        // Set proper permissions for the file
        std::string chmodFileCmd = "chmod 644 \"" + envPath + "\"";
        system(chmodFileCmd.c_str());
    }
    
    std::ifstream inFile(envPath.c_str());
    std::ofstream outFile(tempPath.c_str());
    bool keyFound = false;
    std::string line;
    
    if (!outFile.is_open()) {
        std::cerr << RED "Error: Could not open .env file for writing" RESET << std::endl;
        std::cerr << YELLOW "Debug: Check if ~/.pagestreamer directory is writable with 'ls -la ~/.pagestreamer'" RESET << std::endl;
        std::cerr << YELLOW "Try running: sudo chown -R $USER:$USER ~/.pagestreamer" RESET << std::endl;
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
        std::cerr << YELLOW "Debug: Temporary file exists: " << (access(tempPath.c_str(), F_OK) == 0 ? "Yes" : "No") << RESET << std::endl;
        std::cerr << YELLOW "Debug: Error code: " << errno << " (" << strerror(errno) << ")" RESET << std::endl;
        return false;
    }
    
    return true;
}

/**
 * @brief Displays the current configuration values
 * 
 * Shows the platform, stream URL, and masked stream key
 * 
 * @return bool Always returns true
 */
bool showConfiguration() {
    std::string envPath = std::string(getenv("HOME")) + "/.pagestreamer/.env";
    std::ifstream envFile(envPath.c_str());
    std::string platform = "Not configured";
    std::string streamKey = "Not configured";
    std::string streamUrl = "Not configured";
    
    if (envFile.is_open()) {
        std::string line;
        while (std::getline(envFile, line)) {
            if (line.find("PLATFORM=") == 0 && line.length() > 9) {
                platform = line.substr(9);
            } else if (line.find("STREAM_KEY=") == 0 && line.length() > 11) {
                streamKey = line.substr(11);
                // Mask all but the first and last 4 characters of the stream key
                if (streamKey.length() > 8) {
                    streamKey = streamKey.substr(0, 4) + 
                                std::string(streamKey.length() - 8, '*') + 
                                streamKey.substr(streamKey.length() - 4);
                } else if (streamKey.length() > 0) {
                    // If key is short, mask all but the first and last characters
                    streamKey = streamKey.substr(0, 1) + 
                                std::string(streamKey.length() - 2, '*') + 
                                streamKey.substr(streamKey.length() - 1);
                }
            } else if (line.find("STREAM_URL=") == 0 && line.length() > 11) {
                streamUrl = line.substr(11);
            }
        }
        envFile.close();
    } else {
        std::cout << YELLOW "Configuration file not found or cannot be opened." RESET << std::endl;
        return true;
    }
    
    std::cout << B CYAN "Current Configuration:" RESET << std::endl;
    std::cout << CYAN "Platform: " RESET << platform << std::endl;
    std::cout << CYAN "Stream Key: " RESET << streamKey << std::endl;
    std::cout << CYAN "Stream URL: " RESET << streamUrl << std::endl;
    
    return true;
}

/**
 * @brief Reads a password without echoing it to the console
 * 
 * Uses terminal control characters to disable echo during input
 * 
 * @return string The entered password/key
 */
std::string readPassword() {
    // Special handling for Unix-based systems
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    std::string password;
    std::cin >> password;
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    
    return password;
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
 * @param inConfigSequence Flag to indicate if this is part of a full config sequence
 * @return bool True if configuration was successful
 */
bool configureStreamKey(bool inConfigSequence) {
    std::string streamKey;
    
    // Only use ignore if not in a full configuration sequence
    if (!inConfigSequence) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    
    std::cout << B CYAN "Stream Key Configuration" RESET << std::endl;
    std::cout << YELLOW "Enter your stream key: " RESET;
    std::cout << WHITE "(what you type or paste will be hidden for security but it exists, just paste with CTRL+SHIFT+V on linux then press ENTER)" RESET << std::endl;
    
    // Use password-style input to hide the stream key as it's typed
    streamKey = readPassword();
    
    if (streamKey.empty()) {
        std::cout << RED "Stream key cannot be empty" RESET << std::endl;
        return false;
    }
    
    return updateEnvFile("STREAM_KEY", streamKey);
}

/**
 * @brief Configures the website URL to be streamed
 * 
 * Prompts the user to enter the URL of the website they want to stream
 * 
 * @return bool True if configuration was successful
 */
bool configureStreamUrl() {
    std::string streamUrl;
    
    // We don't need to ignore here since readPassword() leaves the buffer in a clean state
    
    std::cout << B CYAN "Stream URL Configuration" RESET << std::endl;
    std::cout << YELLOW "Enter the URL of the website you want to stream: " RESET;
    std::cin >> streamUrl;
    
    if (streamUrl.empty()) {
        std::cout << YELLOW "Using default: https://example.com/" RESET << std::endl;
        streamUrl = "https://example.com/";
    }
    
    // Validate URL format (basic validation)
    if (streamUrl.find("http") != 0) {
        std::cout << YELLOW "URL should start with http:// or https://, adding https://" RESET << std::endl;
        streamUrl = "https://" + streamUrl;
    }
    
    return updateEnvFile("STREAM_URL", streamUrl);
}

/**
 * @brief Handles the configuration of stream settings
 * 
 * @param configType The specific setting to configure (optional)
 * @return bool True if configuration was successful
 */
bool handleConfig(const std::string& configType) {
    bool success = true;
    
    if (configType == "see") {
        return showConfiguration();
    }
    
    if (configType.empty() || configType == "PLATFORM") {
        success = configurePlatform() && success;
    }
    
    if (configType.empty() || configType == "STREAM_KEY") {
        // When doing a full config, pass true to indicate we're in a sequence
        if (configType.empty()) {
            success = configureStreamKey(true) && success;
        } else {
            success = configureStreamKey(false) && success;
        }
    }
    
    if (configType.empty() || configType == "STREAM_URL") {
        success = configureStreamUrl() && success;
    }
    
    if (success) {
        std::cout << GREEN "Configuration saved successfully!" RESET << std::endl;
        std::cout << "You can change settings anytime with: pagestreamer --config" << std::endl;
        std::cout << "You can view current settings with: pagestreamer --config see" << std::endl;
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
                return handleConfig("") ? 0 : 1;
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