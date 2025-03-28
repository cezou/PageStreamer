#include "../includes/ConfigManager.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <limits>
#include <unistd.h>
#include <termios.h>

/**
 * @brief Constructor
 * 
 * Initializes the ConfigManager with the path to the .env file.
 */
ConfigManager::ConfigManager() {
    envPath = std::string(getenv("HOME")) + "/.pagestreamer/.env";
}

/**
 * @brief Destructor
 */
ConfigManager::~ConfigManager() {
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
bool ConfigManager::updateEnvFile(const std::string& key, const std::string& value) {
    std::string tempPath = envPath + ".tmp";
    std::string dirPath = std::string(getenv("HOME")) + "/.pagestreamer";
    std::string mkdirCmd = "mkdir -p \"" + dirPath + "\"";
    std::string chmodCmd = "chmod 755 \"" + dirPath + "\"";
    system(mkdirCmd.c_str());
    system(chmodCmd.c_str());
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
bool ConfigManager::showConfiguration() {
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
                if (streamKey.length() > 8) {
                    streamKey = streamKey.substr(0, 4) + 
                                std::string(streamKey.length() - 8, '*') + 
                                streamKey.substr(streamKey.length() - 4);
                } else if (streamKey.length() > 0) {
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
 * @brief Handles the configuration of stream settings
 * 
 * @param configType The specific setting to configure (optional)
 * @return bool True if configuration was successful
 */
bool ConfigManager::handleConfig(const std::string& configType) {
    bool success = true;
    if (configType == "see") {
        return showConfiguration();
    }
    if (configType.empty() || configType == "PLATFORM") {
        success = configurePlatform() && success;
    }
    if (configType.empty() || configType == "STREAM_KEY") {
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
