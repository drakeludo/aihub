#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>
#include <string>

class Logger {
public:
    static Logger& instance() {
        static Logger instance;
        return instance;
    }
    
    void initialize();
    
    template<typename... Args>
    void info(Args&&... args) {
        if (logger_) logger_->info(std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void warning(Args&&... args) {
        if (logger_) logger_->warn(std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void error(Args&&... args) {
        if (logger_) logger_->error(std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void debug(Args&&... args) {
        if (logger_) logger_->debug(std::forward<Args>(args)...);
    }
    
private:
    Logger() = default;
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    std::shared_ptr<spdlog::logger> logger_;
};
