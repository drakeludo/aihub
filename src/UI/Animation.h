#pragma once

#include <chrono>
#include <functional>
#include <vector>
#include <memory>

enum class EaseType {
    Linear,
    EaseIn,
    EaseOut,
    EaseInOut,
    Bounce,
    Elastic
};

class Animation {
public:
    Animation(float duration, EaseType ease = EaseType::EaseInOut)
        : duration_(duration), ease_(ease) {
        startTime_ = std::chrono::steady_clock::now();
    }
    
    bool isComplete() const {
        return getCurrentTime() >= duration_;
    }
    
    float getProgress() const {
        float t = getCurrentTime() / duration_;
        if (t >= 1.0f) return 1.0f;
        if (t <= 0.0f) return 0.0f;
        return applyEase(t);
    }
    
    float getValue(float start, float end) const {
        return start + (end - start) * getProgress();
    }
    
    void reset() {
        startTime_ = std::chrono::steady_clock::now();
    }
    
private:
    float getCurrentTime() const {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime_);
        return elapsed.count() / 1000.0f;
    }
    
    float applyEase(float t) const {
        switch (ease_) {
            case EaseType::Linear:
                return t;
            case EaseType::EaseIn:
                return t * t;
            case EaseType::EaseOut:
                return t * (2.0f - t);
            case EaseType::EaseInOut:
                return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;
            case EaseType::Bounce:
                if (t < (1.0f / 2.75f)) {
                    return 7.5625f * t * t;
                } else if (t < (2.0f / 2.75f)) {
                    t -= (1.5f / 2.75f);
                    return 7.5625f * t * t + 0.75f;
                } else if (t < (2.5f / 2.75f)) {
                    t -= (2.25f / 2.75f);
                    return 7.5625f * t * t + 0.9375f;
                } else {
                    t -= (2.625f / 2.75f);
                    return 7.5625f * t * t + 0.984375f;
                }
            case EaseType::Elastic:
                if (t == 0.0f || t == 1.0f) return t;
                return std::pow(2.0f, -10.0f * t) * std::sin((t - 0.075f) * (2.0f * 3.14159f) / 0.3f) + 1.0f;
        }
        return t;
    }
    
    float duration_;
    EaseType ease_;
    std::chrono::steady_clock::time_point startTime_;
};

class AnimationManager {
public:
    static AnimationManager& instance() {
        static AnimationManager instance;
        return instance;
    }
    
    std::shared_ptr<Animation> create(float duration, EaseType ease = EaseType::EaseInOut) {
        auto anim = std::make_shared<Animation>(duration, ease);
        animations_.push_back(anim);
        return anim;
    }
    
    void update() {
        animations_.erase(
            std::remove_if(animations_.begin(), animations_.end(),
                [](const std::weak_ptr<Animation>& weak) {
                    return weak.expired();
                }),
            animations_.end()
        );
    }
    
    void clear() {
        animations_.clear();
    }
    
private:
    AnimationManager() = default;
    ~AnimationManager() = default;
    AnimationManager(const AnimationManager&) = delete;
    AnimationManager& operator=(const AnimationManager&) = delete;
    
    std::vector<std::weak_ptr<Animation>> animations_;
};
