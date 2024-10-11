#ifndef LOGGER
#define LOGGER

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <filesystem>

namespace fs = std::filesystem;

enum class LogLevel {
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    Logger(const std::string& strFilePath) {
        fs::path logFilePath(strFilePath);

        if (!fs::exists(logFilePath.parent_path())) {
            std::cerr <<  "Log file directory does not exist: " << logFilePath.string() << std::endl;
        }

        logFile.open(logFilePath, std::ios::app);

        if (!logFile.is_open()) {
            std::cerr << "Failed to open log file: " << logFilePath.string() << std::endl;
        }else{
            logFile << "\n-------- New session --------" << std::endl;
        }
    }

    ~Logger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    void log(const std::string& message, LogLevel level = LogLevel::INFO) {
        std::string logMessage = getCurrentTime() + " " + logLevelToString(level) + ": " + message;
        if (logFile.is_open()) {
            logFile << logMessage << std::endl;
        } else {
            std::cerr << "Log file not open. Logging to console instead." << std::endl;
            std::cout << logMessage << std::endl;
        }
    }

private:
    std::ofstream logFile;

    // Get the current time as a string
    std::string getCurrentTime() {
        std::time_t now = std::time(0);
        char buf[80];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        return std::string(buf);
    }

    // Convert LogLevel enum to string
    std::string logLevelToString(LogLevel level) {
        switch (level) {
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARNING: return "WARNING";
            case LogLevel::ERROR: return "ERROR";
            default: return "UNKNOWN";
        }
    }
};

#endif

//int main() {
//    try {
//        // Create logger with a path using std::filesystem
//        Logger logger(fs::path("logs/logfile.txt"));
//
//        logger.log("This is an info message.", LogLevel::INFO);
//        logger.log("This is a warning message.", LogLevel::WARNING);
//        logger.log("This is an error message.", LogLevel::ERROR);
//    } catch (const std::exception& e) {
//        std::cerr << "Exception: " << e.what() << std::endl;
//    }
//
//    return 0;
//}
