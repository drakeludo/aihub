#pragma once

#include <QObject>
#include <QStringList>
#include <memory>
#include <map>
#include "../Core/AIRequest.h"

class AIService;
class ChatWidget;
class DatabaseManager;
class WorkspaceManager;

class ChatController : public QObject {
    Q_OBJECT
    
public:
    explicit ChatController(QObject *parent = nullptr);
    ~ChatController() override;
    
    // Initialize controller
    void initialize();
    
    // Chat actions
    void createNewChat();
    void loadChat(int chatId);
    void deleteChat(int chatId);
    void clearChat(int chatId);
    
    // Message actions
    void sendMessage(const QString& message);
    void editMessage(int messageId, const QString& newContent);
    void deleteMessage(int messageId);
    void regenerateResponse(int messageId);
    
    // File actions
    void attachFile(const QString& filePath);
    void removeAttachment(const QString& filePath);
    
    // Export actions
    void exportChatAsMarkdown(int chatId, const QString& filePath);
    void exportChatAsJSON(int chatId, const QString& filePath);
    void exportChatAsTXT(int chatId, const QString& filePath);
    
    // Settings
    void setProvider(AIProvider provider);
    void setSystemPrompt(const QString& prompt);
    void setProcessingStrategy(ProcessingStrategy strategy);
    
    // Getters
    int currentChatId() const { return currentChatId_; }
    AIProvider currentProvider() const { return currentProvider_; }
    
signals:
    void chatCreated(int chatId);
    void chatLoaded(int chatId);
    void chatDeleted(int chatId);
    
    void messageSent(int messageId);
    void messageReceived(int messageId, const QString& content);
    void messageError(const QString& error);
    
    void processingStarted();
    void processingProgress(int progress, const QString& stage);
    void processingCompleted();
    
private:
    struct PendingRequestInfo {
        int messageId{-1};
        int chatId{-1};
    };

    void onRequestCompleted(const QString& requestId, const AIResponse& response);
    void onRequestFailed(const QString& requestId, const QString& error);
    void onRequestProgress(const QString& requestId, int progress, const QString& stage);
    
    std::unique_ptr<AIService> aiService_;
    std::unique_ptr<DatabaseManager> database_;
    std::unique_ptr<WorkspaceManager> workspaceManager_;
    
    int currentChatId_{-1};
    int currentProjectId_{-1};
    AIProvider currentProvider_{AIProvider::Auto};
    QString currentSystemPrompt_;
    QStringList currentAttachments_;
    
    std::map<QString, PendingRequestInfo> requestToMessageMap_;
};
