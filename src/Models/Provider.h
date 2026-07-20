#pragma once

#include <string>
#include <vector>

enum class ProviderType {
    ChatGPT,
    DeepSeek,
    Claude,
    Gemini,
    Kiro,
    Custom
};

enum class ProviderStatus {
    Offline,
    NotAuthorized,
    Ready,
    Busy,
    Error
};

struct Provider {
    ProviderType type;
    std::string name;
    std::string displayName;
    std::string url;
    std::string iconPath;
    ProviderStatus status{ProviderStatus::Offline};
    std::vector<std::string> availableModels;
    std::string currentModel;
    bool requiresAuth{true};
    bool isEnabled{true};
    
    // Visual
    unsigned int accentColor{0xFF6B4FFF}; // RGBA
    
    Provider(ProviderType t, const std::string& n, const std::string& dn, const std::string& u)
        : type(t), name(n), displayName(dn), url(u) {}
};
