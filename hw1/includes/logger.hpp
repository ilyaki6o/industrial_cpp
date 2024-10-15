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

void createPath(fs::path path){
    if (fs::exists(path)){
        return;
    }else{
        createPath(path.parent_path());
    }

    fs::create_directories(path);

    return;
}

class Logger {
public:
    Logger(const std::string& strFileDir) : logFileDir(strFileDir){
        if (!fs::exists(logFileDir)) {
            createPath(logFileDir);
        }

        fs::path logFilePath = logFileDir / "start_game.log";

        logFile.open(logFilePath, std::ios::trunc);

        if (!logFile.is_open()) {
            std::cerr << "Failed to open log file: " << logFilePath.string() << std::endl;
        }else{
            logFile << "-------- New session --------" << std::endl;
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

    void newRound(int roundNumb){
        logFile.close();

        fs::path logFilePath = logFileDir / ("round" + std::to_string(roundNumb) + ".log");

        logFile.open(logFilePath, std::ios::trunc);

        if (!logFile.is_open()) {
            std::cerr << "Failed to open log file: " << logFilePath.string() << std::endl;
        }else{
            logFile << "-------- " << "Round" + std::to_string(roundNumb) << " --------" << std::endl;
        }
    }

    void nightPhase(){
        if (logFile.is_open()){
            logFile << "~~~City falling asleep~~~" << std::endl;
        }
    }

    void dayPhase(){
        if (logFile.is_open()){
            logFile << "~~~City waking up~~~" << std::endl;
        }
    }

    void final(const std::string& message){
        logFile.close();

        fs::path logFilePath = logFileDir / "final_result.log";

        logFile.open(logFilePath, std::ios::trunc);

        if (!logFile.is_open()) {
            std::cerr << "Failed to open log file: " << logFilePath.string() << std::endl;
        }else{
            logFile << message << std::endl;
        }

    }

private:
    std::ofstream logFile;
    fs::path logFileDir;

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
