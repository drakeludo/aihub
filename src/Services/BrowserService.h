#pragma once

#include <string>
#include <functional>
#include <memory>
#include <Windows.h>
#include <wrl.h>
#include "WebView2.h"

using namespace Microsoft::WRL;

// Provider types
enum class AIProvider {
    ChatGPT,
    Claude,
    DeepSeek,
    Gemini
};

class BrowserService {
public:
    static BrowserService& instance() {
        static BrowserService inst;
        return inst;
    }

    // Initialize WebView2
    bool initialize(HWND parentWindow);
    void shutdown();

    // Navigation
    void navigateToProvider(AIProvider provider);
    void reload();
    void goBack();
    void goForward();

    // Message interaction
    void sendMessage(const std::string& message);
    void getResponse(std::function<void(const std::string&)> callback);

    // WebView control
    void setVisible(bool visible);
    void resize(int x, int y, int width, int height);
    
    // State
    bool isInitialized() const { return webView_ != nullptr; }
    bool isLoading() const { return isLoading_; }
    AIProvider getCurrentProvider() const { return currentProvider_; }

private:
    BrowserService() = default;
    ~BrowserService() { shutdown(); }

    BrowserService(const BrowserService&) = delete;
    BrowserService& operator=(const BrowserService&) = delete;

    // WebView2 members
    ComPtr<ICoreWebView2Controller> controller_;
    ComPtr<ICoreWebView2> webView_;
    HWND parentHwnd_{nullptr};
    
    // State
    bool isLoading_{false};
    AIProvider currentProvider_{AIProvider::ChatGPT};
    
    // URLs for different providers
    std::string getProviderURL(AIProvider provider);
    
    // JavaScript injection for automation
    void injectAutomationScript();
};
