#pragma once

#include "../Models/Conversation.h"
#include "../Models/ChatMessage.h"
#include <string>
#include <vector>
#include <memory>

// Forward declare SQLite
struct sqlite3;

class HistoryService {
public:
    static HistoryService& instance() {
        static HistoryService instance;
        return instance;
    }
    
    bool initialize(const std::string& dbPath);
    void shutdown();
    
    // Conversations
    int saveConversation(const Conversation& conv);
    bool updateConversation(const Conversation& conv);
    bool deleteConversation(int id);
    Conversation loadConversation(int id);
    std::vector<Conversation> loadAllConversations();
    std::vector<Conversation> searchConversations(const std::string& query);
    
    // Messages
    int saveMessage(const ChatMessage& msg);
    bool updateMessage(const ChatMessage& msg);
    bool deleteMessage(int id);
    std::vector<ChatMessage> loadMessages(int conversationId);
    
    // Settings
    bool saveSetting(const std::string& key, const std::string& value);
    std::string loadSetting(const std::string& key, const std::string& defaultValue = "");
    
    // Statistics
    int getTotalConversations();
    int getTotalMessages();
    int getTotalTokens();
    
private:
    HistoryService();
    ~HistoryService();
    HistoryService(const HistoryService&) = delete;
    HistoryService& operator=(const HistoryService&) = delete;
    
    bool createTables();
    bool executeSQL(const std::string& sql);
    
    sqlite3* db_{nullptr};
    bool initialized_{false};
};
