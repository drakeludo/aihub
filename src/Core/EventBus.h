#pragma once

#include "Event.h"
#include <algorithm>
#include <functional>
#include <vector>
#include <mutex>
#include <unordered_map>

using EventCallback = std::function<void(const Event&)>;

class EventBus {
public:
    static EventBus& instance() {
        static EventBus instance;
        return instance;
    }
    
    // Subscribe to event
    int subscribe(EventType type, EventCallback callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        int id = nextId_++;
        subscribers_[type].push_back({id, callback});
        return id;
    }
    
    // Unsubscribe
    void unsubscribe(int id) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& [type, callbacks] : subscribers_) {
            callbacks.erase(
                std::remove_if(callbacks.begin(), callbacks.end(),
                    [id](const auto& pair) { return pair.first == id; }),
                callbacks.end()
            );
        }
    }
    
    // Publish event
    void publish(const Event& event) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = subscribers_.find(event.type);
        if (it != subscribers_.end()) {
            for (const auto& [id, callback] : it->second) {
                callback(event);
            }
        }
    }
    
    // Helper to create and publish
    void emit(EventType type) {
        Event event(type);
        publish(event);
    }
    
    template<typename T>
    void emit(EventType type, const std::string& key, const T& value) {
        Event event(type);
        event.set(key, value);
        publish(event);
    }
    
private:
    EventBus() = default;
    ~EventBus() = default;
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;
    
    std::mutex mutex_;
    int nextId_{0};
    std::unordered_map<EventType, std::vector<std::pair<int, EventCallback>>> subscribers_;
};
