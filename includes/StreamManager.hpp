#ifndef STREAM_MANAGER_HPP
# define STREAM_MANAGER_HPP

# include <string>
# include <stdexcept>
# include <iostream>
# include <cstdlib>
# include "Colors.hpp"

/**
 * @brief Manages the streaming service to various platforms
 * 
 * This class provides methods to control the streaming process,
 * including starting, stopping and checking the status.
 */
class StreamManager {
private:
    std::string scriptPath;
    
public:
    StreamManager();
    ~StreamManager();
    
    bool startStream();
    bool stopStream();
    bool getStreamStatus();
};

#endif
