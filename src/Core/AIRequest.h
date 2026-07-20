#pragma once

#include <QString>
#include <QStringList>
#include <chrono>
#include <memory>

enum class AIProvider {
    DeepSeek,
    ChatGPT,
    Kiro,
    Claude,
    Gemini,
    Auto  // Let AIService decide
};

enum class RequestPriority {
    Low,
    Normal,
    High,
    Urgent
};

struct AIRequest {
    // Required
    QString prompt;
    AIProvider provider{AIProvider::Auto};
    
    // Optional
    QString systemPrompt;
    int chatId{-1};
    int projectId{-1};
    QStringList attachments;
    RequestPriority priority{RequestPriority::Normal};
    std::chrono::milliseconds timeout{180000};  // 3 minutes
    int maxRetries{3};
    
    // Metadata
    QString requestId;
    std::chrono::system_clock::time_point timestamp;
    
    // Callback
    std::function<void(const QString& response)> onSuccess;
    std::function<void(const QString& error)> onError;
    std::function<void(int progress)> onProgress;
    
    AIRequest() {
        timestamp = std::chrono::system_clock::now();
    }
};

struct AIResponse {
    QString content;
    AIProvider provider;
    std::chrono::milliseconds duration;
    bool success{false};
    QString error;
    QString requestId;
    
    QString providerName() const {
        switch (provider) {
            case AIProvider::DeepSeek: return "DeepSeek";
            case AIProvider::ChatGPT: return "ChatGPT";
            case AIProvider::Kiro: return "Kiro";
            case AIProvider::Claude: return "Claude";
            case AIProvider::Gemini: return "Gemini";
            default: return "Unknown";
        }
    }
};
