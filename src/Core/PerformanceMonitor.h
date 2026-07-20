#pragma once

#include <chrono>
#include <deque>
#include <numeric>

class PerformanceMonitor {
public:
    static PerformanceMonitor& instance() {
        static PerformanceMonitor instance;
        return instance;
    }
    
    void beginFrame() {
        frameStart_ = std::chrono::high_resolution_clock::now();
    }
    
    void endFrame() {
        auto frameEnd = std::chrono::high_resolution_clock::now();
        auto frameDuration = std::chrono::duration_cast<std::chrono::microseconds>(frameEnd - frameStart_);
        
        frameTimes_.push_back(frameDuration.count() / 1000.0f); // Convert to ms
        if (frameTimes_.size() > maxSamples_) {
            frameTimes_.pop_front();
        }
        
        frameCount_++;
    }
    
    float getFPS() const {
        if (frameTimes_.empty()) return 0.0f;
        float avgFrameTime = getAverageFrameTime();
        return avgFrameTime > 0.0f ? 1000.0f / avgFrameTime : 0.0f;
    }
    
    float getAverageFrameTime() const {
        if (frameTimes_.empty()) return 0.0f;
        return std::accumulate(frameTimes_.begin(), frameTimes_.end(), 0.0f) / frameTimes_.size();
    }
    
    float getMinFrameTime() const {
        if (frameTimes_.empty()) return 0.0f;
        return *std::min_element(frameTimes_.begin(), frameTimes_.end());
    }
    
    float getMaxFrameTime() const {
        if (frameTimes_.empty()) return 0.0f;
        return *std::max_element(frameTimes_.begin(), frameTimes_.end());
    }
    
    const std::deque<float>& getFrameTimes() const {
        return frameTimes_;
    }
    
    size_t getFrameCount() const {
        return frameCount_;
    }
    
    void reset() {
        frameTimes_.clear();
        frameCount_ = 0;
    }
    
private:
    PerformanceMonitor() = default;
    ~PerformanceMonitor() = default;
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    
    std::chrono::high_resolution_clock::time_point frameStart_;
    std::deque<float> frameTimes_;
    size_t frameCount_{0};
    static constexpr size_t maxSamples_ = 100;
};
