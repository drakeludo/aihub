#include "AIService.h"
#include "../Browser/BrowserManager.h"
#include "../Services/ConversationManager.h"
#include "../Services/PromptManager.h"
#include "../Logger/Logger.h"
#include <QUuid>

AIService::AIService(QObject *parent)
    : QObject(parent)
    , browserManager_(std::make_unique<BrowserManager>())
    , conversationManager_(std::make_unique<ConversationManager>())
    , promptManager_(std::make_unique<PromptManager>())
{
}

AIService::~AIService() {
    running_ = false;
    queueCV_.notify_all();
    if (workerThread_.joinable()) {
        workerThread_.join();
    }
}

void AIService::initialize() {
    Logger::instance().info("AIService: Initializing...");
    
    // Initialize browser manager
    browserManager_->initialize();
    
    // Start worker thread
    running_ = true;
    workerThread_ = std::thread(&AIService::workerThread, this);
    
    Logger::instance().info("AIService: Initialized");
}

void AIService::processRequest(const AIRequest& request) {
    // Generate request ID if not present
    AIRequest req = request;
    if (req.requestId.isEmpty()) {
        req.requestId = QUuid::createUuid().toString();
    }
    
    Logger::instance().info("AIService: Queuing request " + req.requestId.toStdString());
    
    std::lock_guard<std::mutex> lock(queueMutex_);
    requestQueue_.push(req);
    queueCV_.notify_one();
}

void AIService::setStrategy(ProcessingStrategy strategy) {
    strategy_ = strategy;
    Logger::instance().info("AIService: Strategy changed to " + 
                          std::to_string(static_cast<int>(strategy)));
}

void AIService::cancelRequest(const QString& requestId) {
    cancelledRequests_[requestId] = true;
    Logger::instance().info("AIService: Request cancelled " + requestId.toStdString());
}

int AIService::queueSize() const {
    std::lock_guard<std::mutex> lock(queueMutex_);
    return static_cast<int>(requestQueue_.size());
}

void AIService::workerThread() {
    Logger::instance().info("AIService: Worker thread started");
    
    while (running_) {
        std::unique_lock<std::mutex> lock(queueMutex_);
        queueCV_.wait(lock, [this] { 
            return !requestQueue_.empty() || !running_; 
        });
        
        if (!running_) break;
        
        if (!requestQueue_.empty()) {
            AIRequest request = requestQueue_.front();
            requestQueue_.pop();
            lock.unlock();
            
            processRequestInternal(request);
        }
    }
    
    Logger::instance().info("AIService: Worker thread stopped");
}

void AIService::processRequestInternal(const AIRequest& request) {
    // Check if cancelled
    if (cancelledRequests_[request.requestId]) {
        emit requestFailed(request.requestId, "Request cancelled");
        cancelledRequests_.erase(request.requestId);
        return;
    }
    
    emit requestStarted(request.requestId);
    
    try {
        AIResponse response;
        
        // Select strategy
        switch (strategy_) {
            case ProcessingStrategy::Single:
                response = processSingle(request);
                break;
            case ProcessingStrategy::Chain:
                response = processChain(request);
                break;
            case ProcessingStrategy::Parallel:
                response = processParallel(request);
                break;
            case ProcessingStrategy::Best:
                response = processBest(request);
                break;
        }
        
        if (response.success) {
            // Save to conversation
            conversationManager_->addMessage(request.chatId, "user", request.prompt);
            conversationManager_->addMessage(request.chatId, "assistant", response.content);
            
            // Call success callback
            if (request.onSuccess) {
                request.onSuccess(response.content);
            }
            
            emit requestCompleted(request.requestId, response);
        } else {
            if (request.onError) {
                request.onError(response.error);
            }
            emit requestFailed(request.requestId, response.error);
        }
        
    } catch (const std::exception& e) {
        Logger::instance().error("AIService: Request failed - " + std::string(e.what()));
        if (request.onError) {
            request.onError(e.what());
        }
        emit requestFailed(request.requestId, e.what());
    }
}

AIResponse AIService::processSingle(const AIRequest& request) {
    Logger::instance().info("AIService: Processing single request");
    
    AIProvider provider = request.provider;
    if (provider == AIProvider::Auto) {
        provider = selectProvider(request);
    }
    
    emit requestProgress(request.requestId, 50, "Processing...");
    
    // Use synchronous sendPrompt
    QString response = browserManager_->sendPrompt(provider, request.prompt);
    
    AIResponse result;
    result.content = response;
    result.provider = provider;
    result.success = !response.startsWith("Error:");
    result.requestId = request.requestId;
    
    return result;
}

AIResponse AIService::processChain(const AIRequest& request) {
    Logger::instance().info("AIService: Processing chain request");
    
    // Step 1: DeepSeek
    emit requestProgress(request.requestId, 25, "DeepSeek analyzing...");
    QString deepseekResponse = browserManager_->sendPrompt(AIProvider::DeepSeek, request.prompt);
    
    if (deepseekResponse.startsWith("Error:")) {
        AIResponse result;
        result.success = false;
        result.error = deepseekResponse;
        return result;
    }
    
    // Step 2: GPT improves
    emit requestProgress(request.requestId, 75, "ChatGPT improving...");
    QString gptPrompt = "Improve and expand this response:\n\n" + deepseekResponse + 
                       "\n\nOriginal question: " + request.prompt;
    QString gptResponse = browserManager_->sendPrompt(AIProvider::ChatGPT, gptPrompt);
    
    AIResponse result;
    result.content = gptResponse;
    result.provider = AIProvider::ChatGPT;
    result.success = !gptResponse.startsWith("Error:");
    result.requestId = request.requestId;
    
    return result;
}

AIResponse AIService::processParallel(const AIRequest& request) {
    Logger::instance().info("AIService: Processing parallel request");
    
    // TODO: Implement parallel processing
    // For now, fallback to chain
    return processChain(request);
}

AIResponse AIService::processBest(const AIRequest& request) {
    Logger::instance().info("AIService: Processing best request");
    
    // TODO: Try multiple providers and select best
    // For now, fallback to chain
    return processChain(request);
}

AIProvider AIService::selectProvider(const AIRequest& request) {
    // Simple selection logic
    // TODO: Implement smart provider selection based on:
    // - Provider capabilities
    // - Request type
    // - Provider availability
    // - User preferences
    
    return AIProvider::DeepSeek;
}
