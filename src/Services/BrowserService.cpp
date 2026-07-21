#include "BrowserService.h"
#include "../Logger/Logger.h"
#include <wil/com.h>

bool BrowserService::initialize(HWND parentWindow) {
    if (webView_) {
        Logger::instance().warning("BrowserService already initialized");
        return true;
    }

    parentHwnd_ = parentWindow;
    Logger::instance().info("Initializing WebView2...");

    // Create WebView2 environment and controller
    HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(
        nullptr, nullptr, nullptr,
        Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [this](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {
                if (FAILED(result)) {
                    Logger::instance().error("Failed to create WebView2 environment: {}", result);
                    return result;
                }

                env->CreateCoreWebView2Controller(parentHwnd_,
                    Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                        [this](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
                            if (FAILED(result)) {
                                Logger::instance().error("Failed to create WebView2 controller: {}", result);
                                return result;
                            }

                            controller_ = controller;
                            controller_->get_CoreWebView2(&webView_);

                            // Set default size
                            RECT bounds;
                            GetClientRect(parentHwnd_, &bounds);
                            controller_->put_Bounds(bounds);

                            // Setup event handlers
                            webView_->add_NavigationCompleted(
                                Callback<ICoreWebView2NavigationCompletedHandler>(
                                    [this](ICoreWebView2* sender, ICoreWebView2NavigationCompletedEventArgs* args) -> HRESULT {
                                        isLoading_ = false;
                                        Logger::instance().info("WebView2 navigation completed");
                                        
                                        // Inject automation script
                                        injectAutomationScript();
                                        
                                        return S_OK;
                                    }).Get(),
                                nullptr);

                            Logger::instance().info("WebView2 initialized successfully");
                            
                            // Navigate to default provider
                            navigateToProvider(AIProvider::ChatGPT);

                            return S_OK;
                        }).Get());

                return S_OK;
            }).Get());

    return SUCCEEDED(hr);
}

void BrowserService::shutdown() {
    if (controller_) {
        controller_->Close();
        controller_ = nullptr;
    }
    webView_ = nullptr;
    Logger::instance().info("BrowserService shutdown");
}

std::string BrowserService::getProviderURL(AIProvider provider) {
    switch (provider) {
        case AIProvider::ChatGPT:
            return "https://chat.openai.com/";
        case AIProvider::Claude:
            return "https://claude.ai/";
        case AIProvider::DeepSeek:
            return "https://chat.deepseek.com/";
        case AIProvider::Gemini:
            return "https://gemini.google.com/";
        default:
            return "https://chat.openai.com/";
    }
}

void BrowserService::navigateToProvider(AIProvider provider) {
    if (!webView_) {
        Logger::instance().error("WebView2 not initialized");
        return;
    }

    currentProvider_ = provider;
    std::string url = getProviderURL(provider);
    
    Logger::instance().info("Navigating to: {}", url);
    isLoading_ = true;
    
    std::wstring wurl(url.begin(), url.end());
    webView_->Navigate(wurl.c_str());
}

void BrowserService::openBrowser(const std::string& providerName) {
    AIProvider provider = AIProvider::ChatGPT;
    
    if (providerName == "chatgpt" || providerName == "ChatGPT") {
        provider = AIProvider::ChatGPT;
    } else if (providerName == "claude" || providerName == "Claude") {
        provider = AIProvider::Claude;
    } else if (providerName == "deepseek" || providerName == "DeepSeek") {
        provider = AIProvider::DeepSeek;
    } else if (providerName == "gemini" || providerName == "Gemini") {
        provider = AIProvider::Gemini;
    } else if (providerName == "kiro" || providerName == "Kiro") {
        // Kiro - special case (local or custom URL)
        if (webView_) {
            std::wstring kiroUrl = L"http://localhost:7777"; // Adjust as needed
            webView_->Navigate(kiroUrl.c_str());
            Logger::instance().info("Opening Kiro at localhost:7777");
        }
        return;
    }
    
    navigateToProvider(provider);
    setVisible(true); // Make WebView visible
}

void BrowserService::reload() {
    if (webView_) {
        webView_->Reload();
    }
}

void BrowserService::goBack() {
    if (webView_) {
        BOOL canGoBack;
        webView_->get_CanGoBack(&canGoBack);
        if (canGoBack) {
            webView_->GoBack();
        }
    }
}

void BrowserService::goForward() {
    if (webView_) {
        BOOL canGoForward;
        webView_->get_CanGoForward(&canGoForward);
        if (canGoForward) {
            webView_->GoForward();
        }
    }
}

void BrowserService::sendMessage(const std::string& message) {
    if (!webView_) {
        Logger::instance().error("WebView2 not initialized");
        return;
    }

    // JavaScript to send message (provider-specific)
    std::string jsCode;
    
    switch (currentProvider_) {
        case AIProvider::ChatGPT:
            jsCode = R"(
                (function() {
                    const textarea = document.querySelector('#prompt-textarea, textarea[placeholder*="Message"]');
                    if (textarea) {
                        textarea.value = ')" + message + R"(';
                        textarea.dispatchEvent(new Event('input', { bubbles: true }));
                        
                        // Find and click send button
                        setTimeout(() => {
                            const sendBtn = document.querySelector('button[data-testid="send-button"]');
                            if (sendBtn && !sendBtn.disabled) {
                                sendBtn.click();
                            }
                        }, 100);
                    }
                })();
            )";
            break;
            
        case AIProvider::Claude:
            jsCode = R"(
                (function() {
                    const textarea = document.querySelector('div[contenteditable="true"]');
                    if (textarea) {
                        textarea.textContent = ')" + message + R"(';
                        textarea.dispatchEvent(new Event('input', { bubbles: true }));
                        
                        setTimeout(() => {
                            const sendBtn = document.querySelector('button[type="submit"]');
                            if (sendBtn) {
                                sendBtn.click();
                            }
                        }, 100);
                    }
                })();
            )";
            break;
            
        default:
            Logger::instance().warning("Message sending not implemented for this provider");
            return;
    }

    std::wstring wjs(jsCode.begin(), jsCode.end());
    webView_->ExecuteScript(wjs.c_str(), nullptr);
    
    Logger::instance().info("Message sent to provider");
}

void BrowserService::getResponse(std::function<void(const std::string&)> callback) {
    if (!webView_) return;

    // JavaScript to extract response
    std::string jsCode = R"(
        (function() {
            const messages = document.querySelectorAll('[data-message-author-role="assistant"]');
            if (messages.length > 0) {
                const lastMsg = messages[messages.length - 1];
                return lastMsg.textContent;
            }
            return '';
        })();
    )";

    std::wstring wjs(jsCode.begin(), jsCode.end());
    webView_->ExecuteScript(wjs.c_str(),
        Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
            [callback](HRESULT result, LPCWSTR resultObjectAsJson) -> HRESULT {
                if (SUCCEEDED(result) && resultObjectAsJson) {
                    std::wstring wstr(resultObjectAsJson);
                    std::string response(wstr.begin(), wstr.end());
                    callback(response);
                }
                return S_OK;
            }).Get());
}

void BrowserService::injectAutomationScript() {
    if (!webView_) return;

    // Inject helper functions for automation
    std::string jsCode = R"(
        (function() {
            window.AIHubAutomation = {
                ready: true,
                version: '1.0.0'
            };
            console.log('AIHub automation script injected');
        })();
    )";

    std::wstring wjs(jsCode.begin(), jsCode.end());
    webView_->ExecuteScript(wjs.c_str(), nullptr);
}

void BrowserService::setVisible(bool visible) {
    if (controller_) {
        controller_->put_IsVisible(visible ? TRUE : FALSE);
    }
}

void BrowserService::resize(int x, int y, int width, int height) {
    if (controller_) {
        RECT bounds = { x, y, x + width, y + height };
        controller_->put_Bounds(bounds);
    }
}
