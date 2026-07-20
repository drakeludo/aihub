#include "BrowserFactory.h"
#include "../Logger/Logger.h"

void BrowserFactory::registerBrowser(AIProvider provider, BrowserCreator creator) {
    creators_[provider] = creator;
    Logger::instance().info("Browser registered: " + std::to_string(static_cast<int>(provider)));
}

std::unique_ptr<IBrowser> BrowserFactory::createBrowser(AIProvider provider) {
    auto it = creators_.find(provider);
    if (it != creators_.end()) {
        return it->second();
    }
    
    Logger::instance().error("Browser not found: " + std::to_string(static_cast<int>(provider)));
    return nullptr;
}

bool BrowserFactory::isSupported(AIProvider provider) const {
    return creators_.find(provider) != creators_.end();
}

std::vector<AIProvider> BrowserFactory::supportedProviders() const {
    std::vector<AIProvider> providers;
    for (const auto& [provider, _] : creators_) {
        providers.push_back(provider);
    }
    return providers;
}
