#include "BrowserManager.h"
#include "../Core/BrowserFactory.h"
#include "../Logger/Logger.h"
#include <future>

BrowserManager::BrowserManager(QObject *parent)
    : QObject(parent)
{
    Logger::instance().info("BrowserManager created");
}

BrowserManager::~BrowserManager() {
    running_ = false;
    queueCV_.notify_all();
    
    if (workerThread_.joinable()) {
        workerThread_.join();
    }
    
    Logger::instance().info("BrowserManager destroyed");
}

void BrowserManager::initialize() {
    Logger::instance().info("Initializing browsers...");
    
    // Start worker thread
    running_ = true;
    workerThread_ = std::thread(&BrowserManager::workerThread, this);
    
    // Initialize browsers asynchronously
    std::thread([this]() {
        initializeBrowsers();
    }).detach();
}

void BrowserManager::initializeBrowsers() {
    Logger::instance().info("Starting browser initialization...");
    
    try {
        auto& factory = BrowserFactory::instance();
        
        // Get all supported providers
        auto providers = factory.supportedProviders();
        
        for (auto provider : providers) {
            Logger::instance().info("Initializing " + 
                                  std::to_string(static_cast<int>(provider)) + " browser...");
            
            // Create browser
            auto browser = factory.createBrowser(provider);
            if (browser) {
                // Initialize
                browser->initialize();
                
                // Store in map
                browsers_[provider] = std::move(browser);
                
                emit browserReady(provider);
            } else {
                Logger::instance().error("Failed to create browser: " + 
                                       std::to_string(static_cast<int>(provider)));
            }
        }
        
        initialized_ = true;
        Logger::instance().info("All browsers initialized successfully");
        
    } catch (const std::exception& e) {
        Logger::instance().error("Browser initialization failed: " + std::string(e.what()));
        initialized_ = false;
    }
}

QString BrowserManager::sendPrompt(AIProvider provider, const QString& prompt) {
    if (!initialized_) {
        Logger::instance().warning("Browsers not initialized yet");
        return "Error: Browsers not ready";
    }
    
    auto it = browsers_.find(provider);
    if (it == browsers_.end()) {
        Logger::instance().error("Provider not found: " + std::to_string(static_cast<int>(provider)));
        return "Error: Provider not available";
    }
    
    Logger::instance().info("Sending prompt to " + std::to_string(static_cast<int>(provider)));
    
    try {
        QString response = it->second->sendPrompt(prompt);
        emit responseReceived(provider, response);
        return response;
        
    } catch (const std::exception& e) {
        Logger::instance().error("Send prompt failed: " + std::string(e.what()));
        return "Error: " + QString(e.what());
    }
}

void BrowserManager::sendPromptAsync(AIProvider provider, const QString& prompt,
                                     std::function<void(const QString&)> callback) {
    if (!initialized_) {
        Logger::instance().warning("Browsers not initialized yet");
        callback("Error: Browsers not ready");
        return;
    }
    
    Logger::instance().info("Queuing prompt for " + 
                          std::to_string(static_cast<int>(provider)));
    
    std::lock_guard<std::mutex> lock(queueMutex_);
    commandQueue_.push({provider, prompt, callback});
    queueCV_.notify_one();
}

IBrowser* BrowserManager::getBrowser(AIProvider provider) {
    auto it = browsers_.find(provider);
    return (it != browsers_.end()) ? it->second.get() : nullptr;
}

bool BrowserManager::hasProvider(AIProvider provider) const {
    return browsers_.find(provider) != browsers_.end();
}

void BrowserManager::workerThread() {
    Logger::instance().info("Browser worker thread started");
    
    while (running_) {
        std::unique_lock<std::mutex> lock(queueMutex_);
        queueCV_.wait(lock, [this] { 
            return !commandQueue_.empty() || !running_; 
        });
        
        if (!running_) break;
        
        if (!commandQueue_.empty()) {
            BrowserCommand cmd = commandQueue_.front();
            commandQueue_.pop();
            lock.unlock();
            
            processCommand(cmd);
        }
    }
    
    Logger::instance().info("Browser worker thread stopped");
}

void BrowserManager::processCommand(const BrowserCommand& cmd) {
    try {
        auto it = browsers_.find(cmd.provider);
        if (it == browsers_.end()) {
            Logger::instance().error("Provider not found: " + 
                                   std::to_string(static_cast<int>(cmd.provider)));
            cmd.callback("Error: Provider not available");
            emit browserError(cmd.provider, "Provider not available");
            return;
        }
        
        Logger::instance().info("Processing " + 
                              std::to_string(static_cast<int>(cmd.provider)) + " request");
        
        QString response = it->second->sendPrompt(cmd.prompt);
        
        // Call callback
        cmd.callback(response);
        emit responseReceived(cmd.provider, response);
        
    } catch (const std::exception& e) {
        Logger::instance().error("Command processing failed: " + std::string(e.what()));
        cmd.callback("Error: " + QString(e.what()));
        emit browserError(cmd.provider, e.what());
    }
}
