#pragma once

#include <map>
#include <string>
#include <functional>
#include <Windows.h>

using HotkeyCallback = std::function<void()>;

struct Hotkey {
    int vkCode;
    bool ctrl;
    bool shift;
    bool alt;
    
    Hotkey(int vk = 0, bool c = false, bool s = false, bool a = false)
        : vkCode(vk), ctrl(c), shift(s), alt(a) {}
};

class HotkeyManager {
public:
    static HotkeyManager& instance() {
        static HotkeyManager instance;
        return instance;
    }
    
    void registerHotkey(const std::string& name, const Hotkey& key, HotkeyCallback callback) {
        hotkeys_[name] = {key, callback};
    }
    
    void unregisterHotkey(const std::string& name) {
        hotkeys_.erase(name);
    }
    
    bool processInput() {
        bool ctrlPressed = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        bool shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
        bool altPressed = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
        
        for (const auto& [name, data] : hotkeys_) {
            const auto& [key, callback] = data;
            
            if ((GetAsyncKeyState(key.vkCode) & 0x8000) &&
                ctrlPressed == key.ctrl &&
                shiftPressed == key.shift &&
                altPressed == key.alt) {
                
                // Debounce
                if (!wasPressed_[name]) {
                    callback();
                    wasPressed_[name] = true;
                }
                return true;
            } else {
                wasPressed_[name] = false;
            }
        }
        
        return false;
    }
    
    void registerDefaults() {
        // Ctrl+N - New Chat
        registerHotkey("new_chat", Hotkey('N', true, false, false), []() {
            // Will be connected to ChatService
        });
        
        // Ctrl+Enter - Send
        registerHotkey("send", Hotkey(VK_RETURN, true, false, false), []() {
            // Will be connected to ChatWindow
        });
        
        // Ctrl+F - Search
        registerHotkey("search", Hotkey('F', true, false, false), []() {
            // TODO
        });
        
        // Ctrl+, - Settings
        registerHotkey("settings", Hotkey(VK_OEM_COMMA, true, false, false), []() {
            // TODO
        });
        
        // Escape - Cancel
        registerHotkey("cancel", Hotkey(VK_ESCAPE, false, false, false), []() {
            // TODO
        });
    }
    
private:
    HotkeyManager() = default;
    ~HotkeyManager() = default;
    HotkeyManager(const HotkeyManager&) = delete;
    HotkeyManager& operator=(const HotkeyManager&) = delete;
    
    std::map<std::string, std::pair<Hotkey, HotkeyCallback>> hotkeys_;
    std::map<std::string, bool> wasPressed_;
};
