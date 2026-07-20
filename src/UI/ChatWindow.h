#pragma once

#include <string>
#include <vector>

class ChatController;

struct Message {
    std::string sender;
    std::string content;
    enum Type { User, Assistant, System } type;
};

class ChatWindow {
public:
    explicit ChatWindow(ChatController* controller);
    
    void render();
    void addMessage(const std::string& sender, const std::string& content, Message::Type type);
    void clear();
    
private:
    void renderMessageList();
    void renderInputArea();
    
    ChatController* controller_;
    std::vector<Message> messages_;
    char inputBuffer_[4096] = {0};
    bool scrollToBottom_ = false;
};
