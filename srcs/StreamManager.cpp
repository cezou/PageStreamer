#include "../includes/StreamManager.hpp"
#include <cstdlib>
#include <fstream>
#include <string>

/**
 * @brief Constructor
 * 
 * Initializes the StreamManager with the path to the scripts directory.
 */
StreamManager::StreamManager() {
    scriptPath = std::string(getenv("HOME")) + "/.pagestreamer/scripts";
}

/**
 * @brief Destructor
 */
StreamManager::~StreamManager() {
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
 * Verifies that the configuration is complete before starting.
 * 
 * @return True if the stream started successfully, false otherwise
 * @throws std::runtime_error If an error occurs during execution
 */
bool StreamManager::startStream() {
    std::string envPath = std::string(getenv("HOME")) + "/.pagestreamer/.env";
    std::ifstream envFile(envPath.c_str());
    bool hasStreamKey = false;
    bool hasPlatform = false;
    
    if (envFile.is_open()) {
        std::string line;
        while (std::getline(envFile, line)) {
            if (line.find("STREAM_KEY=") == 0 && line.length() > 11) {
                hasStreamKey = true;
            }
            if (line.find("PLATFORM=") == 0 && line.length() > 9) {
                hasPlatform = true;
            }
        }
        envFile.close();
    }
    
    if (!hasStreamKey || !hasPlatform) {
        throw std::runtime_error("Stream configuration incomplete. Run 'pagestreamer --config' first.");
    }
    
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
