#include "Logger.h"
#include <vector>
#include <filesystem>

void Logger::initialize() {
    try {
        // Create logs directory
        std::filesystem::path logsPath = std::filesystem::temp_directory_path() / "AIHub" / "logs";
        std::filesystem::create_directories(logsPath);
        
        std::string logFile = (logsPath / "aihub.log").string();
        
        // Create sinks
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::debug);
        
        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            logFile, 1024 * 1024 * 10, 3  // 10MB, 3 files
        );
        file_sink->set_level(spdlog::level::trace);
        
        std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
        
        // Create logger
        logger_ = std::make_shared<spdlog::logger>("AIHub", sinks.begin(), sinks.end());
        logger_->set_level(spdlog::level::debug);
        logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
        
        spdlog::register_logger(logger_);
        
        info("Logger initialized successfully");
        info("Log file: {}", logFile);
        
    } catch (const std::exception& e) {
        // Fallback to console only
        logger_ = spdlog::stdout_color_mt("AIHub");
        logger_->error("Logger initialization failed: {}", e.what());
    }
}
