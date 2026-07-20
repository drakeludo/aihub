#include "ChatController.h"
#include "../Services/AIService.h"
#include "../Database/DatabaseManager.h"
#include "../Services/WorkspaceManager.h"
#include "../Logger/Logger.h"
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QUuid>

ChatController::ChatController(QObject *parent)
    : QObject(parent)
    , aiService_(std::make_unique<AIService>())
    , database_(std::make_unique<DatabaseManager>())
    , workspaceManager_(std::make_unique<WorkspaceManager>())
{
    Logger::instance().info("ChatController created");
}

ChatController::~ChatController() {
    Logger::instance().info("ChatController destroyed");
}

void ChatController::initialize() {
    Logger::instance().info("ChatController: Initializing...");
    
    // Initialize AI service
    aiService_->initialize();
    
    // Initialize database
    database_->initialize();
    
    // Initialize workspace manager
    workspaceManager_->initialize();
    
    // Connect signals
    connect(aiService_.get(), &AIService::requestStarted,
            this, &ChatController::processingStarted);
    
    connect(aiService_.get(), &AIService::requestProgress,
            this, &ChatController::onRequestProgress);
    
    connect(aiService_.get(), &AIService::requestCompleted,
            this, &ChatController::onRequestCompleted);
    
    connect(aiService_.get(), &AIService::requestFailed,
            this, &ChatController::onRequestFailed);
    
    Logger::instance().info("ChatController: Initialized");
}

void ChatController::createNewChat() {
    Logger::instance().info("ChatController: Creating new chat");
    
    // Create in database
    currentChatId_ = database_->createChat("New Chat");
    
    // Reset state
    currentAttachments_.clear();
    
    emit chatCreated(currentChatId_);
    Logger::instance().info("ChatController: New chat created: " + std::to_string(currentChatId_));
}

void ChatController::loadChat(int chatId) {
    Logger::instance().info("ChatController: Loading chat " + std::to_string(chatId));
    
    currentChatId_ = chatId;
    
    // Clear attachments
    currentAttachments_.clear();
    
    emit chatLoaded(chatId);
}

void ChatController::deleteChat(int chatId) {
    Logger::instance().info("ChatController: Deleting chat " + std::to_string(chatId));
    
    database_->deleteChat(chatId);
    
    if (currentChatId_ == chatId) {
        currentChatId_ = -1;
    }
    
    emit chatDeleted(chatId);
}

void ChatController::clearChat(int chatId) {
    Logger::instance().info("ChatController: Clearing chat " + std::to_string(chatId));
    
    database_->clearMessages(chatId);
}

void ChatController::sendMessage(const QString& message) {
    if (currentChatId_ < 0) {
        createNewChat();
    }
    
    Logger::instance().info("ChatController: Sending message");
    
    // Save user message to database
    int messageId = database_->saveMessage(currentChatId_, "user", message);
    emit messageSent(messageId);
    
    // Create AI request
    AIRequest request;
    request.provider = currentProvider_;
    request.prompt = message;
    request.chatId = currentChatId_;
    request.projectId = currentProjectId_;
    request.systemPrompt = currentSystemPrompt_;
    request.attachments = currentAttachments_;
    request.priority = RequestPriority::Normal;
    request.timeout = std::chrono::milliseconds(180000); // 3 minutes
    request.maxRetries = 3;
    request.requestId = QUuid::createUuid().toString();
    
    // Set callbacks
    request.onSuccess = [this, messageId](const QString& response) {
        // This runs in worker thread, signal will marshal to UI thread
        emit messageReceived(messageId, response);
    };
    
    request.onError = [this](const QString& error) {
        emit messageError(error);
    };
    
    // Map request to message
    requestToMessageMap_[request.requestId] = PendingRequestInfo{messageId, currentChatId_};
    
    // Send to AI service
    aiService_->processRequest(request);
    
    // Clear attachments after sending
    currentAttachments_.clear();
}

void ChatController::editMessage(int messageId, const QString& newContent) {
    Logger::instance().info("ChatController: Editing message " + std::to_string(messageId));
    
    database_->updateMessage(messageId, newContent);
}

void ChatController::deleteMessage(int messageId) {
    Logger::instance().info("ChatController: Deleting message " + std::to_string(messageId));
    
    database_->deleteMessage(messageId);
}

void ChatController::regenerateResponse(int messageId) {
    Logger::instance().info("ChatController: Regenerating response for message " + 
                          std::to_string(messageId));
    
    // Get original message
    auto message = database_->getMessage(messageId);
    if (!message.isEmpty()) {
        sendMessage(message);
    }
}

void ChatController::attachFile(const QString& filePath) {
    Logger::instance().info("ChatController: Attaching file: " + filePath.toStdString());
    
    currentAttachments_.append(filePath);
}

void ChatController::removeAttachment(const QString& filePath) {
    Logger::instance().info("ChatController: Removing attachment: " + filePath.toStdString());
    
    currentAttachments_.removeAll(filePath);
}

void ChatController::exportChatAsMarkdown(int chatId, const QString& filePath) {
    Logger::instance().info("ChatController: Exporting chat as Markdown");
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        Logger::instance().error("Failed to open file: " + filePath.toStdString());
        return;
    }
    
    QTextStream out(&file);
    
    // Write header
    out << "# AI Hub Chat Export\n\n";
    out << "Chat ID: " << chatId << "\n\n";
    out << "---\n\n";
    
    // Get messages
    auto messages = database_->loadMessages(chatId);
    for (const auto& msg : messages) {
        QString role = msg.role;
        QString content = msg.content;
        
        out << "## " << role.toUpper() << "\n\n";
        out << content << "\n\n";
        out << "---\n\n";
    }
    
    file.close();
    Logger::instance().info("Chat exported successfully");
}

void ChatController::exportChatAsJSON(int chatId, const QString& filePath) {
    Logger::instance().info("ChatController: Exporting chat as JSON");
    
    QJsonObject root;
    root["chatId"] = chatId;
    root["exportedAt"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    // Get messages
    auto messages = database_->loadMessages(chatId);
    QJsonArray messagesArray;
    
    for (const auto& msg : messages) {
        QJsonObject msgObj;
        msgObj["id"] = msg.id;
        msgObj["role"] = msg.role;
        msgObj["content"] = msg.content;
        msgObj["timestamp"] = msg.timestamp;
        messagesArray.append(msgObj);
    }
    
    root["messages"] = messagesArray;
    
    // Write to file
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(root);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
        Logger::instance().info("Chat exported successfully");
    } else {
        Logger::instance().error("Failed to open file: " + filePath.toStdString());
    }
}

void ChatController::exportChatAsTXT(int chatId, const QString& filePath) {
    Logger::instance().info("ChatController: Exporting chat as TXT");
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        Logger::instance().error("Failed to open file: " + filePath.toStdString());
        return;
    }
    
    QTextStream out(&file);
    
    // Get messages
    auto messages = database_->loadMessages(chatId);
    for (const auto& msg : messages) {
        out << "[" << msg.role.toUpper() << "]\n";
        out << msg.content << "\n\n";
        out << "----------------------------------------\n\n";
    }
    
    file.close();
    Logger::instance().info("Chat exported successfully");
}

void ChatController::setProvider(AIProvider provider) {
    currentProvider_ = provider;
    Logger::instance().info("ChatController: Provider changed to " + 
                          std::to_string(static_cast<int>(provider)));
}

void ChatController::setSystemPrompt(const QString& prompt) {
    currentSystemPrompt_ = prompt;
    Logger::instance().info("ChatController: System prompt updated");
}

void ChatController::setProcessingStrategy(ProcessingStrategy strategy) {
    aiService_->setStrategy(strategy);
    Logger::instance().info("ChatController: Processing strategy changed");
}

void ChatController::onRequestCompleted(const QString& requestId, const AIResponse& response) {
    Logger::instance().info("ChatController: Request completed: " + requestId.toStdString());
    
    // Find message ID
    auto it = requestToMessageMap_.find(requestId);
    if (it != requestToMessageMap_.end()) {
        const PendingRequestInfo requestInfo = it->second;
        
        // Save AI response to database
        database_->saveMessage(requestInfo.chatId, 
                             response.providerName().toLower(), 
                             response.content);
        
        requestToMessageMap_.erase(it);
    }
    
    emit processingCompleted();
}

void ChatController::onRequestFailed(const QString& requestId, const QString& error) {
    Logger::instance().error("ChatController: Request failed: " + error.toStdString());
    
    emit messageError(error);
    emit processingCompleted();
    
    // Clean up mapping
    requestToMessageMap_.erase(requestId);
}

void ChatController::onRequestProgress(const QString& requestId, int progress, const QString& stage) {
    emit processingProgress(progress, stage);
}
