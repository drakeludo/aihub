#pragma once

#include <QObject>
#include <memory>
#include <queue>
#include <mutex>
#include "../Core/AIRequest.h"

class BrowserManager;
class ConversationManager;
class PromptManager;

enum class ProcessingStrategy {
    Single,      // Single AI
    Chain,       // DeepSeek → GPT
    Parallel,    // Multiple AIs simultaneously
    Best         // Try multiple, return best
};

class AIService : public QObject {
    Q_OBJECT
    
public:
    explicit AIService(QObject *parent = nullptr);
    ~AIService() override;
    
    // Initialize service
    void initialize();
    
    // Process AI request
    void processRequest(const AIRequest& request);
    
    // Set processing strategy
    void setStrategy(ProcessingStrategy strategy);
    
    // Cancel request
    void cancelRequest(const QString& requestId);
    
    // Get queue size
    int queueSize() const;
    
signals:
    void requestStarted(const QString& requestId);
    void requestProgress(const QString& requestId, int progress, const QString& stage);
    void requestCompleted(const QString& requestId, const AIResponse& response);
    void requestFailed(const QString& requestId, const QString& error);
    
private:
    void workerThread();
    void processRequestInternal(const AIRequest& request);
    
    // Processing strategies
    AIResponse processSingle(const AIRequest& request);
    AIResponse processChain(const AIRequest& request);
    AIResponse processParallel(const AIRequest& request);
    AIResponse processBest(const AIRequest& request);
    
    // Select best provider
    AIProvider selectProvider(const AIRequest& request);
    
    std::unique_ptr<BrowserManager> browserManager_;
    std::unique_ptr<ConversationManager> conversationManager_;
    std::unique_ptr<PromptManager> promptManager_;
    
    std::queue<AIRequest> requestQueue_;
    std::mutex queueMutex_;
    std::condition_variable queueCV_;
    
    std::thread workerThread_;
    std::atomic<bool> running_{false};
    
    ProcessingStrategy strategy_{ProcessingStrategy::Chain};
    std::map<QString, bool> cancelledRequests_;
};
