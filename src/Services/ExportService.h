#pragma once

#include "../Models/Conversation.h"
#include <string>

enum class ExportFormat {
    Markdown,
    TXT,
    HTML,
    JSON,
    PDF
};

class ExportService {
public:
    static ExportService& instance() {
        static ExportService instance;
        return instance;
    }
    
    bool exportConversation(const Conversation& conv, ExportFormat format, const std::string& path);
    bool importConversation(const std::string& path, ExportFormat format, Conversation& out);
    
private:
    ExportService() = default;
    ~ExportService() = default;
    ExportService(const ExportService&) = delete;
    ExportService& operator=(const ExportService&) = delete;
    
    std::string toMarkdown(const Conversation& conv);
    std::string toTXT(const Conversation& conv);
    std::string toHTML(const Conversation& conv);
    std::string toJSON(const Conversation& conv);
    bool toPDF(const Conversation& conv, const std::string& path);
    
    bool fromMarkdown(const std::string& content, Conversation& out);
    bool fromTXT(const std::string& content, Conversation& out);
    bool fromJSON(const std::string& content, Conversation& out);
};
