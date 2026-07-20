#include "Logger.h"
#include <ctime>
#include <iomanip>
#include <filesystem>
#include <iostream>

void Logger::initialize() {
    std::lock_guard<std::mutex> lock(mutex_);

    try {
        std::filesystem::path logsPath = std::filesystem::temp_directory_path() / "AIHub" / "logs";
        std::filesystem::create_directories(logsPath);

        std::filesystem::path logPath = logsPath / "aihub.log";
        logFile_.open(logPath, std::ios::out | std::ios::app);
        initialized_ = true;

        std::string line = "[" + currentTimestamp() + "] [INFO] Logger initialized successfully";
        if (logFile_.is_open()) {
            logFile_ << line << '\n';
            logFile_ << "[" + currentTimestamp() + "] [INFO] Log file: " << logPath.string() << '\n';
            logFile_.flush();
        }
        std::clog << line << '\n';
        std::clog << "[" + currentTimestamp() + "] [INFO] Log file: " << logPath.string() << '\n';
    } catch (...) {
        initialized_ = true;
        std::clog << "[" + currentTimestamp() + "] [ERROR] Logger initialization failed\n";
    }
}

Logger::~Logger() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (logFile_.is_open()) {
        logFile_.flush();
        logFile_.close();
    }
}

std::string Logger::currentTimestamp() {
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &time);
#else
    localtime_r(&time, &tm);
#endif

    std::ostringstream stream;
    stream << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return stream.str();
}
