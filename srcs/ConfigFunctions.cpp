#include "../includes/ConfigManager.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <termios.h>
#include <unistd.h>

/**
 * @brief Reads a password without echoing it to the console
 * 
 * Uses terminal control characters to disable echo during input
 * 
 * @return string The entered password/key
 */
std::string readPassword() {
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
    struct PlatformInfo {
        std::string name;
        std::string rtmpUrl;
    };
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
    ConfigManager configManager;
    return configManager.updateEnvFile("PLATFORM", rtmpUrl);
}

/**
 * @brief Configures the streaming key by prompting the user
 * 
 * @param inConfigSequence Flag to indicate if this is part of a full config sequence
 * @return bool True if configuration was successful
 */
bool configureStreamKey(bool inConfigSequence) {
    std::string streamKey;
    if (!inConfigSequence) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cout << B CYAN "Stream Key Configuration" RESET << std::endl;
    std::cout << YELLOW "Enter your stream key: " RESET;
    std::cout << WHITE "(what you type or paste will be hidden for security \
        but it exists, just paste with CTRL+SHIFT+V on linux then press ENTER)\
        " RESET << std::endl;
    streamKey = readPassword();
    if (streamKey.empty()) {
        std::cout << RED "Stream key cannot be empty" RESET << std::endl;
        return false;
    }
    ConfigManager configManager;
    return configManager.updateEnvFile("STREAM_KEY", streamKey);
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
    std::cout << B CYAN "Stream URL Configuration" RESET << std::endl;
    std::cout << YELLOW "Enter the URL of the website you want to stream: " RESET;
    std::cin >> streamUrl;
    if (streamUrl.empty()) {
        std::cout << YELLOW "Using default: https://example.com/" RESET << std::endl;
        streamUrl = "https://example.com/";
    }
    if (streamUrl.find("http") != 0) {
        std::cout << YELLOW "URL should start with http:// or https://, adding https://" RESET << std::endl;
        streamUrl = "https://" + streamUrl;
    }
    ConfigManager configManager;
    return configManager.updateEnvFile("STREAM_URL", streamUrl);
}
