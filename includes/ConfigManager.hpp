#ifndef CONFIG_MANAGER_HPP
# define CONFIG_MANAGER_HPP

# include <string>
# include "Colors.hpp"

/**
 * @brief Manages configuration settings for the streaming service
 * 
 * This class provides methods to read, write, and display configuration
 * settings, including platform, stream key and stream URL.
 */
class ConfigManager {
private:
    std::string envPath;
    
public:
    ConfigManager();
    ~ConfigManager();
    
    bool showConfiguration();
    bool updateEnvFile(const std::string& key, const std::string& value);
    bool handleConfig(const std::string& configType = "");
};

// Function prototypes for configuration functions
bool configurePlatform();
bool configureStreamKey(bool inConfigSequence = false);
bool configureStreamUrl();
std::string readPassword();

#endif
