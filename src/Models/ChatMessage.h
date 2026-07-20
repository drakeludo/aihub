#pragma once

#include <string>
#include <vector>
#include <chrono>

enum class MessageRole {
    User,
    Assistant,
    System
};

enum class MessageStatus {
    Pending,
    Sending,
    Streaming,
    Complete,
    Error,
    Cancelled
};

struct ChatMessage {
    int id{-1};
    int conversationId{-1};
    MessageRole role{MessageRole::User};
    MessageStatus status{MessageStatus::Pending};
    std::string content;
    std::string provider;
    std::string model;
    std::chrono::system_clock::time_point timestamp;
    std::vector<std::string> attachments;
    std::string errorMessage;
    bool isPinned{false};
    
    // Metadata
    int tokenCount{0};
    double generationTime{0.0};
    
    ChatMessage() : timestamp(std::chrono::system_clock::now()) {}
};
