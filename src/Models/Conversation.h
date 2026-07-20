#pragma once

#include <string>
#include <vector>
#include <chrono>
#include "ChatMessage.h"

struct Conversation {
    int id{-1};
    std::string title;
    std::string provider;
    std::string model;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point updatedAt;
    std::vector<ChatMessage> messages;
    bool isPinned{false};
    bool isArchived{false};
    std::string systemPrompt;
    
    // Metadata
    int messageCount{0};
    int totalTokens{0};
    
    Conversation() 
        : createdAt(std::chrono::system_clock::now())
        , updatedAt(std::chrono::system_clock::now()) {}
};
