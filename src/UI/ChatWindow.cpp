#include "ChatWindow.h"
#include "../Services/ChatService.h"
#include "../Services/ThemeService.h"
#include "../Core/EventBus.h"
#include <imgui.h>

ChatWindow::ChatWindow() {
    // Subscribe to message events
    subscriptionId_ = EventBus::instance().subscribe(EventType::MessageSent,
        [this](const Event&) {
            auto* conv = ChatService::instance().getCurrentConversation();
            if (conv && !conv->messages.empty()) {
                addMessage(conv->messages.back());
            }
        });
}

void ChatWindow::render() {
    // Claude Code IDE style: no borders, clean layout
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.11f, 0.11f, 0.11f, 1.0f)); // #1C1C1C
    
    ImGui::Begin("Chat", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    
    auto* conv = ChatService::instance().getCurrentConversation();
    
    if (conv) {
        // Header bar (like Claude Code top bar)
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.09f, 0.09f, 0.09f, 1.0f)); // #171717
        ImGui::BeginChild("HeaderBar", ImVec2(0, 45), false, ImGuiWindowFlags_NoScrollbar);
        
        ImGui::SetCursorPosY(12);
        ImGui::SetCursorPosX(20);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
        ImGui::Text("%s", conv->title.c_str());
        ImGui::PopStyleColor();
        
        ImGui::SameLine(ImGui::GetWindowWidth() - 180);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::Text("%s", conv->model.c_str());
        ImGui::PopStyleColor();
        
        ImGui::EndChild();
        ImGui::PopStyleColor();
        
        // Thin separator
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 p = ImGui::GetCursorScreenPos();
        drawList->AddLine(ImVec2(p.x, p.y), ImVec2(p.x + ImGui::GetWindowWidth(), p.y), 
                         IM_COL32(40, 40, 40, 255), 1.0f);
        
        // Message list
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 1);
        ImGui::BeginChild("MessageList", ImVec2(0, -130), false, ImGuiWindowFlags_NoScrollbar);
        renderMessageList();
        ImGui::EndChild();
        
        // Input area
        renderInputArea();
    } else {
        ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() / 2 - 100, ImGui::GetWindowHeight() / 2 - 20));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::Text("No conversation selected");
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - 100);
        ImGui::Text("Press Ctrl+N to create new chat");
        ImGui::PopStyleColor();
    }
    
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

void ChatWindow::renderMessageList() {
    auto* conv = ChatService::instance().getCurrentConversation();
    if (!conv) return;
    
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 16));
    
    for (const auto& msg : conv->messages) {
        // Claude Code style: minimal, clean messages
        ImGui::SetCursorPosX(60); // Left margin
        
        // Role indicator
        if (msg.role == MessageRole::User) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
            ImGui::Text("You");
            ImGui::PopStyleColor();
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.5f, 0.9f, 1.0f)); // Purple accent
            ImGui::Text("Assistant");
            ImGui::PopStyleColor();
        }
        
        ImGui::SetCursorPosX(60);
        ImGui::Spacing();
        
        // Message content
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
        ImGui::PushTextWrapPos(ImGui::GetWindowWidth() - 80); // Right margin
        ImGui::SetCursorPosX(60);
        ImGui::TextWrapped("%s", msg.content.c_str());
        ImGui::PopTextWrapPos();
        ImGui::PopStyleColor();
        
        // Status
        if (msg.status == MessageStatus::Sending) {
            ImGui::SetCursorPosX(60);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
            ImGui::Text("Generating...");
            ImGui::PopStyleColor();
        } else if (msg.status == MessageStatus::Error) {
            ImGui::SetCursorPosX(60);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
            ImGui::Text("Error: %s", msg.errorMessage.c_str());
            ImGui::PopStyleColor();
        }
        
        ImGui::Spacing();
        ImGui::Spacing();
    }
    
    ImGui::PopStyleVar();
    
    if (scrollToBottom_) {
        ImGui::SetScrollHereY(1.0f);
        scrollToBottom_ = false;
    }
}

void ChatWindow::renderInputArea() {
    // Claude Code style input - clean, minimal
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.09f, 0.09f, 0.09f, 1.0f)); // #171717
    ImGui::BeginChild("InputArea", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar);
    
    ImGui::SetCursorPos(ImVec2(20, 15));
    
    // Input box with border
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.14f, 0.14f, 0.14f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.16f, 0.16f, 0.16f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.18f, 0.18f, 0.18f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 8));
    
    ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 150);
    bool enterPressed = ImGui::InputTextMultiline(
        "##input", inputBuffer_, IM_ARRAYSIZE(inputBuffer_),
        ImVec2(ImGui::GetWindowWidth() - 150, 70),
        ImGuiInputTextFlags_EnterReturnsTrue
    );
    
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(4);
    
    // Send button (Claude style - purple accent)
    ImGui::SameLine();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.3f, 0.7f, 1.0f)); // Purple
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.4f, 0.8f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.5f, 0.9f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    
    if (ImGui::Button("Send", ImVec2(100, 60)) || enterPressed) {
        if (strlen(inputBuffer_) > 0) {
            ChatService::instance().sendMessage(inputBuffer_);
            memset(inputBuffer_, 0, sizeof(inputBuffer_));
            scrollToBottom_ = true;
        }
    }
    
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);
    
    ImGui::EndChild();
    ImGui::PopStyleColor();
}

void ChatWindow::addMessage(const ChatMessage&) {
    scrollToBottom_ = true;
}
