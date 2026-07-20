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
    ImGui::Begin("Chat", nullptr, ImGuiWindowFlags_NoCollapse);
    
    auto* conv = ChatService::instance().getCurrentConversation();
    
    if (conv) {
        // Header
        ImGui::Text("💬 %s", conv->title.c_str());
        ImGui::SameLine(ImGui::GetWindowWidth() - 150);
        ImGui::TextDisabled("Model: %s", conv->model.c_str());
        ImGui::Separator();
        
        // Message list
        ImGui::BeginChild("MessageList", ImVec2(0, -80), true);
        renderMessageList();
        ImGui::EndChild();
        
        // Input area
        renderInputArea();
    } else {
        ImGui::TextDisabled("No active conversation");
        ImGui::TextDisabled("Create a new chat from File menu");
    }
    
    ImGui::End();
}

void ChatWindow::renderMessageList() {
    auto* conv = ChatService::instance().getCurrentConversation();
    if (!conv) return;
    
    for (const auto& msg : conv->messages) {
        ImVec4 bgColor, textColor;
        const char* prefix = "";
        
        if (msg.role == MessageRole::User) {
            bgColor = ImVec4(0.17f, 0.32f, 0.47f, 1.0f);
            textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
            prefix = "👤 You";
        } else if (msg.role == MessageRole::Assistant) {
            bgColor = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);
            textColor = ImVec4(0.88f, 0.88f, 0.88f, 1.0f);
            prefix = "🤖 AI";
        } else {
            bgColor = ImVec4(0.24f, 0.18f, 0.18f, 1.0f);
            textColor = ImVec4(0.80f, 0.80f, 0.80f, 1.0f);
            prefix = "⚙️ System";
        }
        
        ImGui::PushStyleColor(ImGuiCol_ChildBg, bgColor);
        ImGui::BeginChild(("msg_" + std::to_string(msg.id)).c_str(), 
                         ImVec2(0, 0), true, 
                         ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);
        
        // Sender
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.72f, 0.30f, 1.0f));
        ImGui::Text("%s", prefix);
        ImGui::PopStyleColor();
        
        ImGui::Spacing();
        
        // Content
        ImGui::PushStyleColor(ImGuiCol_Text, textColor);
        ImGui::TextWrapped("%s", msg.content.c_str());
        ImGui::PopStyleColor();
        
        // Status indicator
        if (msg.status == MessageStatus::Sending) {
            ImGui::SameLine();
            ImGui::TextDisabled("...");
        } else if (msg.status == MessageStatus::Error) {
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Error: %s", msg.errorMessage.c_str());
        }
        
        ImGui::EndChild();
        ImGui::PopStyleColor();
        
        ImGui::Spacing();
    }
    
    if (scrollToBottom_) {
        ImGui::SetScrollHereY(1.0f);
        scrollToBottom_ = false;
    }
}

void ChatWindow::renderInputArea() {
    ImGui::Separator();
    ImGui::Spacing();
    
    // Input text
    ImGui::SetNextItemWidth(-100);
    ImGui::InputTextMultiline(
        "##input", inputBuffer_, IM_ARRAYSIZE(inputBuffer_),
        ImVec2(-100, 60),
        ImGuiInputTextFlags_CtrlEnterForNewLine
    );
    const bool submitShortcut =
        ImGui::IsItemFocused() &&
        ImGui::GetIO().KeyCtrl &&
        ImGui::IsKeyPressed(ImGuiKey_Enter, false);
    
    // Send button
    ImGui::SameLine();
    if (ImGui::Button("Send\n▶", ImVec2(90, 60)) || submitShortcut) {
        if (strlen(inputBuffer_) > 0) {
            ChatService::instance().sendMessage(inputBuffer_);
            memset(inputBuffer_, 0, sizeof(inputBuffer_));
            scrollToBottom_ = true;
        }
    }
    
    ImGui::Spacing();
}

void ChatWindow::addMessage(const ChatMessage&) {
    scrollToBottom_ = true;
}
