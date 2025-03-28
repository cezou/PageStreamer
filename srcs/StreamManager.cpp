#include "../includes/StreamManager.hpp"

/**
 * @brief Constructor
 * 
 * Initializes the StreamManager with the path to the scripts directory.
 */
StreamManager::StreamManager() {
    scriptPath = "./scripts";
}

/**
 * @brief Destructor
 */
StreamManager::~StreamManager() {
    // Nothing to clean up
}

/**
 * @brief Executes a shell command and returns the result
 * 
 * @param command The command to execute
 * @return int The exit code of the command
 */
static int executeCommand(const std::string& command) {
    return system(command.c_str());
}

/**
 * @brief Starts the stream
 * 
 * Executes the start_stream.sh script to initialize and start the stream.
 * 
 * @return True if the stream started successfully, false otherwise
 * @throws std::runtime_error If an error occurs during execution
 */
bool StreamManager::startStream() {
    std::string command = "cd " + scriptPath + " && bash start_stream.sh";
    int result = executeCommand(command);
    if (result != 0) {
        throw std::runtime_error("Failed to start stream. Check logs for details.");
    }
    return true;
}

/**
 * @brief Stops the stream
 * 
 * Executes the stop_stream.sh script to properly terminate all stream processes.
 * 
 * @return True if the stream stopped successfully, false otherwise
 * @throws std::runtime_error If an error occurs during execution
 */
bool StreamManager::stopStream() {
    std::string command = "cd " + scriptPath + " && bash stop_stream.sh";
    int result = executeCommand(command);
    if (result != 0) {
        throw std::runtime_error("Failed to stop stream. Check logs for details.");
    }
    return true;
}

/**
 * @brief Checks the status of the stream
 * 
 * Executes the status_stream.sh script to get information about the 
 * current stream status.
 * 
 * @return True if the stream is running, false otherwise
 * @throws std::runtime_error If an error occurs during execution
 */
bool StreamManager::getStreamStatus() {
    std::string command = "cd " + scriptPath + " && bash status_stream.sh";
    int result = executeCommand(command);
    return (result == 0);
}
