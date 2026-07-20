#pragma once

#include <string>
#include <any>
#include <chrono>
#include <map>

enum class EventType {
    // Message events
    MessageSent,
    MessageReceived,
    MessageUpdated,
    MessageDeleted,
    StreamingStarted,
    StreamingChunk,
    StreamingComplete,
    
    // Browser events
    BrowserReady,
    BrowserBusy,
    BrowserError,
    BrowserCrashed,
    AuthRequired,
    AuthLost,
    
    // Conversation events
    ConversationCreated,
    ConversationLoaded,
    ConversationDeleted,
    ConversationUpdated,
    
    // System events
    LoadComplete,
    Error,
    Warning,
    Info,
    
    // UI events
    ThemeChanged,
    SettingsChanged
};

struct Event {
    EventType type;
    std::map<std::string, std::any> data;
    std::chrono::system_clock::time_point timestamp;
    
    Event(EventType t) : type(t), timestamp(std::chrono::system_clock::now()) {}
    
    template<typename T>
    void set(const std::string& key, const T& value) {
        data[key] = value;
    }
    
    template<typename T>
    T get(const std::string& key) const {
        auto it = data.find(key);
        if (it != data.end()) {
            return std::any_cast<T>(it->second);
        }
        return T{};
    }
    
    bool has(const std::string& key) const {
        return data.find(key) != data.end();
    }
};
