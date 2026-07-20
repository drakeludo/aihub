#pragma once

#include "IBrowser.h"
#include <memory>
#include <map>
#include <functional>

class BrowserFactory {
public:
    static BrowserFactory& instance() {
        static BrowserFactory instance;
        return instance;
    }
    
    // Register browser creator
    using BrowserCreator = std::function<std::unique_ptr<IBrowser>()>;
    void registerBrowser(AIProvider provider, BrowserCreator creator);
    
    // Create browser instance
    std::unique_ptr<IBrowser> createBrowser(AIProvider provider);
    
    // Check if provider supported
    bool isSupported(AIProvider provider) const;
    
    // Get all supported providers
    std::vector<AIProvider> supportedProviders() const;
    
private:
    BrowserFactory() = default;
    std::map<AIProvider, BrowserCreator> creators_;
};
