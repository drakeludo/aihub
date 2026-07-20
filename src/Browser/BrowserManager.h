#pragma once

#include <QObject>
#include <QString>
#include <memory>
#include <map>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "../Core/IBrowser.h"
#include "../Core/AIRequest.h"

struct BrowserCommand {
    AIProvider provider;
    QString prompt;
    std::function<void(const QString&)> callback;
};

class BrowserManager : public QObject {
    Q_OBJECT
    
public:
    explicit BrowserManager(QObject *parent = nullptr);
    ~BrowserManager() override;
    
    // Initialize all browsers
    void initialize();
    
    // Check if ready
    bool isReady() const { return initialized_; }
    
    // Send prompt to AI (synchronous, blocks until response)
    QString sendPrompt(AIProvider provider, const QString& prompt);
    
    // Send prompt async
    void sendPromptAsync(AIProvider provider, const QString& prompt,
                        std::function<void(const QString&)> callback);
    
    // Get browser instance
    IBrowser* getBrowser(AIProvider provider);
    
    // Check if provider available
    bool hasProvider(AIProvider provider) const;
    
signals:
    void browserReady(AIProvider provider);
    void browserError(AIProvider provider, const QString& error);
    void responseReceived(AIProvider provider, const QString& response);
    
private:
    void initializeBrowsers();
    void workerThread();
    void processCommand(const BrowserCommand& cmd);
    
    // Browser instances (using interface)
    std::map<AIProvider, std::unique_ptr<IBrowser>> browsers_;
    
    // Command queue
    std::queue<BrowserCommand> commandQueue_;
    std::mutex queueMutex_;
    std::condition_variable queueCV_;
    
    // Worker thread
    std::thread workerThread_;
    std::atomic<bool> running_{false};
    std::atomic<bool> initialized_{false};
};
