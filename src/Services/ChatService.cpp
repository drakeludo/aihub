#include "ChatService.h"
#include "../Logger/Logger.h"

ChatService::ChatService() {
    // Subscribe to events
    subscriptionId_ = EventBus::instance().subscribe(EventType::MessageReceived,
        [this](const Event& e) { onMessageReceived(e); });
}

int ChatService::createConversation(const std::string& provider, const std::string& model) {
    Conversation conv;
    conv.id = static_cast<int>(conversations_.size());
    conv.provider = provider;
    conv.model = model;
    conv.title = "New Chat";
    
    conversations_.push_back(conv);
    currentConversationId_ = conv.id;
    
    Logger::instance().info("Created conversation {}", conv.id);
    EventBus::instance().emit(EventType::ConversationCreated, "id", conv.id);
    
    return conv.id;
}

bool ChatService::loadConversation(int id) {
    if (id < 0 || id >= static_cast<int>(conversations_.size())) {
        return false;
    }
    
    currentConversationId_ = id;
    EventBus::instance().emit(EventType::ConversationLoaded, "id", id);
    
    return true;
}

bool ChatService::deleteConversation(int id) {
    if (id < 0 || id >= static_cast<int>(conversations_.size())) {
        return false;
    }
    
    conversations_.erase(conversations_.begin() + id);
    
    if (currentConversationId_ == id) {
        currentConversationId_ = -1;
    }
    
    EventBus::instance().emit(EventType::ConversationDeleted, "id", id);
    
    return true;
}

std::vector<Conversation> ChatService::getConversations() {
    return conversations_;
}

Conversation* ChatService::getCurrentConversation() {
    if (currentConversationId_ < 0 || currentConversationId_ >= static_cast<int>(conversations_.size())) {
        return nullptr;
    }
    return &conversations_[currentConversationId_];
}

int ChatService::sendMessage(const std::string& content, const std::vector<std::string>& attachments) {
    auto* conv = getCurrentConversation();
    if (!conv) {
        Logger::instance().error("No active conversation");
        return -1;
    }
    
    ChatMessage msg;
    msg.id = static_cast<int>(conv->messages.size());
    msg.conversationId = currentConversationId_;
    msg.role = MessageRole::User;
    msg.content = content;
    msg.attachments = attachments;
    msg.status = MessageStatus::Sending;
    
    conv->messages.push_back(msg);
    conv->messageCount++;
    conv->updatedAt = std::chrono::system_clock::now();
    
    Logger::instance().info("Sending message {} in conversation {}", msg.id, currentConversationId_);
    EventBus::instance().emit(EventType::MessageSent, "messageId", msg.id);
    
    // TODO: Send to BrowserService
    
    return msg.id;
}

bool ChatService::cancelMessage(int messageId) {
    (void)messageId; // Suppress unused parameter warning
    // TODO: Cancel browser request
    EventBus::instance().emit(EventType::MessageUpdated, "messageId", 0);
    return true;
}

bool ChatService::regenerateMessage(int messageId) {
    (void)messageId; // Suppress unused parameter warning
    // TODO: Resend request
    return true;
}

bool ChatService::editMessage(int messageId, const std::string& newContent) {
    auto* conv = getCurrentConversation();
    if (!conv) return false;
    
    for (auto& msg : conv->messages) {
        if (msg.id == messageId) {
            msg.content = newContent;
            EventBus::instance().emit(EventType::MessageUpdated, "messageId", messageId);
            return true;
        }
    }
    
    return false;
}

bool ChatService::deleteMessage(int messageId) {
    auto* conv = getCurrentConversation();
    if (!conv) return false;
    
    conv->messages.erase(
        std::remove_if(conv->messages.begin(), conv->messages.end(),
            [messageId](const ChatMessage& msg) { return msg.id == messageId; }),
        conv->messages.end()
    );
    
    EventBus::instance().emit(EventType::MessageDeleted, "messageId", messageId);
    
    return true;
}

bool ChatService::pinMessage(int messageId) {
    auto* conv = getCurrentConversation();
    if (!conv) return false;
    
    for (auto& msg : conv->messages) {
        if (msg.id == messageId) {
            msg.isPinned = !msg.isPinned;
            EventBus::instance().emit(EventType::MessageUpdated, "messageId", messageId);
            return true;
        }
    }
    
    return false;
}

std::vector<ChatMessage> ChatService::searchMessages(const std::string& query) {
    std::vector<ChatMessage> results;
    auto* conv = getCurrentConversation();
    if (!conv) return results;
    
    for (const auto& msg : conv->messages) {
        if (msg.content.find(query) != std::string::npos) {
            results.push_back(msg);
        }
    }
    
    return results;
}

bool ChatService::exportConversation(int id, const std::string& format, const std::string& path) {
    (void)format; // Suppress unused parameter warning
    // TODO: Implement export
    Logger::instance().info("Exporting conversation {} to {}", id, path);
    return true;
}

void ChatService::setSystemPrompt(const std::string& prompt) {
    auto* conv = getCurrentConversation();
    if (conv) {
        conv->systemPrompt = prompt;
    }
}

void ChatService::onMessageReceived(const Event& event) {
    (void)event; // Suppress unused parameter warning
    Logger::instance().info("Message received event");
}

void ChatService::onStreamingChunk(const Event& event) {
    (void)event; // Suppress unused parameter warning
    if (!streamingEnabled_) return;
    
    // TODO: Update message with streaming chunk
}
