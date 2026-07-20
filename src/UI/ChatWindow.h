#pragma once

#include "../Models/ChatMessage.h"
#include <vector>

class ChatWindow {
public:
    ChatWindow();
    void render();
    
private:
    void renderMessageList();
    void renderInputArea();
    void addMessage(const ChatMessage& msg);
    
    std::vector<ChatMessage> displayMessages_;
    char inputBuffer_[4096] = {0};
    bool scrollToBottom_{false};
    int subscriptionId_{-1};
};
