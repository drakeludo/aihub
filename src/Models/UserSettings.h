#pragma once

#include <string>
#include <map>

enum class Theme {
    Cyber,
    Neon,
    Matrix,
    Synthwave,
    TechLab
};

enum class Language {
    English,
    Russian,
    Chinese,
    Spanish
};

struct UserSettings {
    // Appearance
    Theme theme{Theme::Cyber};
    Language language{Language::English};
    float uiScale{1.0f};
    int fontSize{16};
    bool enableAnimations{true};
    bool enableParticles{true};
    bool enableBloom{true};
    
    // Behavior
    bool autoSave{true};
    bool streamResponse{true};
    int maxHistoryItems{100};
    std::string exportFolder;
    
    // Performance
    bool showFPS{true};
    bool showMemory{true};
    int maxThreads{4};
    bool enableCache{true};
    
    // Logging
    int logLevel{2}; // 0=Error, 1=Warning, 2=Info, 3=Debug
    bool logToFile{true};
    bool logToConsole{false};
    
    // Hotkeys
    std::map<std::string, std::string> hotkeys;
    
    UserSettings() {
        // Default hotkeys
        hotkeys["new_chat"] = "Ctrl+N";
        hotkeys["send"] = "Ctrl+Enter";
        hotkeys["search"] = "Ctrl+F";
        hotkeys["settings"] = "Ctrl+,";
        hotkeys["cancel"] = "Escape";
    }
};
