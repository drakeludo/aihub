#include "ExportService.h"
#include "../Logger/Logger.h"
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>

bool ExportService::exportConversation(const Conversation& conv, ExportFormat format, const std::string& path) {
    Logger::instance().info("Exporting conversation to: {}", path);
    
    std::string content;
    
    switch (format) {
        case ExportFormat::Markdown:
            content = toMarkdown(conv);
            break;
        case ExportFormat::TXT:
            content = toTXT(conv);
            break;
        case ExportFormat::HTML:
            content = toHTML(conv);
            break;
        case ExportFormat::JSON:
            content = toJSON(conv);
            break;
        case ExportFormat::PDF:
            return toPDF(conv, path);
    }
    
    std::ofstream file(path);
    if (!file.is_open()) {
        Logger::instance().error("Cannot open file for writing: {}", path);
        return false;
    }
    
    file << content;
    file.close();
    
    return true;
}

bool ExportService::importConversation(const std::string& path, ExportFormat format, Conversation& out) {
    std::ifstream file(path);
    if (!file.is_open()) {
        Logger::instance().error("Cannot open file for reading: {}", path);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    switch (format) {
        case ExportFormat::Markdown:
            return fromMarkdown(content, out);
        case ExportFormat::TXT:
            return fromTXT(content, out);
        case ExportFormat::JSON:
            return fromJSON(content, out);
        default:
            return false;
    }
}

std::string ExportService::toMarkdown(const Conversation& conv) {
    std::ostringstream oss;
    
    oss << "# " << conv.title << "\n\n";
    oss << "**Provider:** " << conv.provider << "\n";
    oss << "**Model:** " << conv.model << "\n";
    oss << "**Messages:** " << conv.messages.size() << "\n\n";
    oss << "---\n\n";
    
    for (const auto& msg : conv.messages) {
        std::string role = msg.role == MessageRole::User ? "👤 **You**" : "🤖 **AI**";
        oss << role << "\n\n";
        oss << msg.content << "\n\n";
        oss << "---\n\n";
    }
    
    return oss.str();
}

std::string ExportService::toTXT(const Conversation& conv) {
    std::ostringstream oss;
    
    oss << conv.title << "\n";
    oss << "Provider: " << conv.provider << "\n";
    oss << "Model: " << conv.model << "\n";
    oss << "Messages: " << conv.messages.size() << "\n";
    oss << "========================================\n\n";
    
    for (const auto& msg : conv.messages) {
        std::string role = msg.role == MessageRole::User ? "You" : "AI";
        oss << "[" << role << "]\n";
        oss << msg.content << "\n\n";
        oss << "----------------------------------------\n\n";
    }
    
    return oss.str();
}

std::string ExportService::toHTML(const Conversation& conv) {
    std::ostringstream oss;
    
    oss << "<!DOCTYPE html>\n";
    oss << "<html><head>\n";
    oss << "<meta charset='utf-8'>\n";
    oss << "<title>" << conv.title << "</title>\n";
    oss << "<style>\n";
    oss << "body { font-family: Arial; background: #0a0a0f; color: #e0e0e0; padding: 20px; }\n";
    oss << ".message { margin: 20px 0; padding: 15px; border-radius: 8px; }\n";
    oss << ".user { background: #2c3e50; }\n";
    oss << ".assistant { background: #1a1a2e; }\n";
    oss << ".role { font-weight: bold; color: #ff006b; margin-bottom: 10px; }\n";
    oss << "</style>\n";
    oss << "</head><body>\n";
    oss << "<h1>" << conv.title << "</h1>\n";
    oss << "<p><strong>Provider:</strong> " << conv.provider << " | <strong>Model:</strong> " << conv.model << "</p>\n";
    oss << "<hr>\n";
    
    for (const auto& msg : conv.messages) {
        std::string cssClass = msg.role == MessageRole::User ? "user" : "assistant";
        std::string role = msg.role == MessageRole::User ? "👤 You" : "🤖 AI";
        
        oss << "<div class='message " << cssClass << "'>\n";
        oss << "<div class='role'>" << role << "</div>\n";
        oss << "<div>" << msg.content << "</div>\n";
        oss << "</div>\n";
    }
    
    oss << "</body></html>";
    
    return oss.str();
}

std::string ExportService::toJSON(const Conversation& conv) {
    std::ostringstream oss;
    
    oss << "{\n";
    oss << "  \"title\": \"" << conv.title << "\",\n";
    oss << "  \"provider\": \"" << conv.provider << "\",\n";
    oss << "  \"model\": \"" << conv.model << "\",\n";
    oss << "  \"messages\": [\n";
    
    for (size_t i = 0; i < conv.messages.size(); i++) {
        const auto& msg = conv.messages[i];
        std::string role = msg.role == MessageRole::User ? "user" : "assistant";
        
        oss << "    {\n";
        oss << "      \"role\": \"" << role << "\",\n";
        oss << "      \"content\": \"" << msg.content << "\"\n";
        oss << "    }";
        
        if (i < conv.messages.size() - 1) oss << ",";
        oss << "\n";
    }
    
    oss << "  ]\n";
    oss << "}";
    
    return oss.str();
}

bool ExportService::toPDF(const Conversation& conv, const std::string& path) {
    // TODO: PDF generation requires external library
    Logger::instance().warning("PDF export not yet implemented");
    return false;
}

bool ExportService::fromMarkdown(const std::string& content, Conversation& out) {
    // TODO: Parse markdown
    return false;
}

bool ExportService::fromTXT(const std::string& content, Conversation& out) {
    // TODO: Parse TXT
    return false;
}

bool ExportService::fromJSON(const std::string& content, Conversation& out) {
    // TODO: Parse JSON
    return false;
}
