#ifndef STREAM_MANAGER_HPP
# define STREAM_MANAGER_HPP

# include <string>
# include <stdexcept>
# include <iostream>
# include <cstdlib>
# include "Colors.hpp"

/**
 * @brief Manages the YouTube streaming service
 * 
 * This class provides methods to start, stop and check the status of
 * the YouTube stream. It uses shell scripts to interact with the 
 * underlying streaming infrastructure.
 */
class StreamManager {
private:
    std::string scriptPath;
    
public:
    /**
     * @brief Constructor
     * 
     * Initializes the StreamManager with the path to the scripts directory.
     */
    StreamManager();
    
    /**
     * @brief Destructor
     */
    ~StreamManager();
    
    /**
     * @brief Starts the stream
     * 
     * Executes the start_stream.sh script to initialize and start the stream.
     * 
     * @return True if the stream started successfully, false otherwise
     * @throws std::runtime_error If an error occurs during execution
     */
    bool startStream();
    
    /**
     * @brief Stops the stream
     * 
     * Executes the stop_stream.sh script to properly terminate all stream processes.
     * 
     * @return True if the stream stopped successfully, false otherwise
     * @throws std::runtime_error If an error occurs during execution
     */
    bool stopStream();
    
    /**
     * @brief Checks the status of the stream
     * 
     * Executes the status_stream.sh script to get information about the 
     * current stream status.
     * 
     * @return True if the stream is running, false otherwise
     * @throws std::runtime_error If an error occurs during execution
     */
    bool getStreamStatus();
};

#endif
