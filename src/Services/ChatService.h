#pragma once

#include "../Models/ChatMessage.h"
#include "../Models/Conversation.h"
#include "../Core/EventBus.h"
#include <memory>
#include <vector>
#include <functional>

class ChatService {
public:
    static ChatService& instance() {
        static ChatService instance;
        return instance;
    }
    
    // Conversation management
    int createConversation(const std::string& provider, const std::string& model);
    bool loadConversation(int id);
    bool deleteConversation(int id);
    std::vector<Conversation> getConversations();
    Conversation* getCurrentConversation();
    
    // Message operations
    int sendMessage(const std::string& content, const std::vector<std::string>& attachments = {});
    bool cancelMessage(int messageId);
    bool regenerateMessage(int messageId);
    bool editMessage(int messageId, const std::string& newContent);
    bool deleteMessage(int messageId);
    bool pinMessage(int messageId);
    
    // Search
    std::vector<ChatMessage> searchMessages(const std::string& query);
    
    // Export
    bool exportConversation(int id, const std::string& format, const std::string& path);
    
    // Settings
    void setStreamingEnabled(bool enabled) { streamingEnabled_ = enabled; }
    void setSystemPrompt(const std::string& prompt);
    
private:
    ChatService();
    ~ChatService() = default;
    ChatService(const ChatService&) = delete;
    ChatService& operator=(const ChatService&) = delete;
    
    void onMessageReceived(const Event& event);
    void onStreamingChunk(const Event& event);
    
    std::vector<Conversation> conversations_;
    int currentConversationId_{-1};
    bool streamingEnabled_{true};
    
    int subscriptionId_{-1};
};
