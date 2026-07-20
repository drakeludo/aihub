#pragma once

#include <chrono>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

class Logger {
public:
    static Logger& instance() {
        static Logger instance;
        return instance;
    }
    
    void initialize();
    
    template<typename... Args>
    void info(Args&&... args) {
        log("INFO", std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void warning(Args&&... args) {
        log("WARN", std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void error(Args&&... args) {
        log("ERROR", std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void debug(Args&&... args) {
        log("DEBUG", std::forward<Args>(args)...);
    }
    
private:
    template<typename T>
    static std::string toString(T&& value) {
        using Decayed = std::decay_t<T>;

        if constexpr (std::is_same_v<Decayed, std::string>) {
            return value;
        } else if constexpr (std::is_same_v<Decayed, std::string_view>) {
            return std::string(value);
        } else if constexpr (std::is_same_v<Decayed, const char*> || std::is_same_v<Decayed, char*>) {
            return value ? std::string(value) : std::string();
        } else if constexpr (std::is_same_v<Decayed, bool>) {
            return value ? "true" : "false";
        } else if constexpr (std::is_enum_v<Decayed>) {
            using Underlying = std::underlying_type_t<Decayed>;
            return std::to_string(static_cast<Underlying>(value));
        } else {
            std::ostringstream stream;
            stream << std::forward<T>(value);
            return stream.str();
        }
    }

    template<typename... Args>
    static std::string formatMessage(std::string format, Args&&... args) {
        if constexpr (sizeof...(Args) == 0) {
            return format;
        } else {
            std::string values[] = {toString(std::forward<Args>(args))...};
            size_t argIndex = 0;
            size_t pos = 0;

            while ((pos = format.find("{}", pos)) != std::string::npos && argIndex < std::size(values)) {
                format.replace(pos, 2, values[argIndex++]);
                pos += values[argIndex - 1].size();
            }

            while (argIndex < std::size(values)) {
                format += ' ';
                format += values[argIndex++];
            }

            return format;
        }
    }

    template<typename... Args>
    void log(std::string_view level, Args&&... args) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!initialized_) {
            return;
        }

        std::string line = "[" + currentTimestamp() + "] [" + std::string(level) + "] " +
                           formatMessage(std::forward<Args>(args)...);

        if (logFile_.is_open()) {
            logFile_ << line << '\n';
            logFile_.flush();
        }

        std::clog << line << '\n';
    }

    static std::string currentTimestamp();

    Logger() = default;
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    std::mutex mutex_;
    std::ofstream logFile_;
    bool initialized_{false};
};
