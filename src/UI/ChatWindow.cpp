#include "ChatWindow.h"
#include "Controllers/ChatController.h"
#include "imgui.h"

ChatWindow::ChatWindow(ChatController* controller)
    : controller_(controller)
{
}

void ChatWindow::render() {
    ImGui::Begin("Chat", nullptr, ImGuiWindowFlags_NoCollapse);
    
    // Message list area (takes most space)
    ImGui::BeginChild("MessageList", ImVec2(0, -70), true);
    renderMessageList();
    ImGui::EndChild();
    
    // Input area (fixed height at bottom)
    renderInputArea();
    
    ImGui::End();
}

void ChatWindow::renderMessageList() {
    for (const auto& msg : messages_) {
        // Message bubble
        ImVec4 bgColor, textColor;
        
        if (msg.type == Message::User) {
            bgColor = ImVec4(0.17f, 0.32f, 0.47f, 1.0f);  // Blue
            textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        } else if (msg.type == Message::Assistant) {
            bgColor = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);  // Dark grey
            textColor = ImVec4(0.88f, 0.88f, 0.88f, 1.0f);
        } else {
            bgColor = ImVec4(0.24f, 0.18f, 0.18f, 1.0f);  // Dark red
            textColor = ImVec4(0.80f, 0.80f, 0.80f, 1.0f);
        }
        
        ImGui::PushStyleColor(ImGuiCol_ChildBg, bgColor);
        ImGui::BeginChild(("msg_" + std::to_string((size_t)&msg)).c_str(), 
                         ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);
        
        // Sender
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.72f, 0.30f, 1.0f));
        ImGui::TextWrapped("%s", msg.sender.c_str());
        ImGui::PopStyleColor();
        
        ImGui::Spacing();
        
        // Content
        ImGui::PushStyleColor(ImGuiCol_Text, textColor);
        ImGui::TextWrapped("%s", msg.content.c_str());
        ImGui::PopStyleColor();
        
        ImGui::EndChild();
        ImGui::PopStyleColor();
        
        ImGui::Spacing();
    }
    
    // Auto-scroll to bottom
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
    bool enterPressed = ImGui::InputTextMultiline("##input", inputBuffer_, IM_ARRAYSIZE(inputBuffer_),
                                                   ImVec2(-100, 50),
                                                   ImGuiInputTextFlags_CtrlEnterForNewLine);
    
    // Send button
    ImGui::SameLine();
    if (ImGui::Button("Send", ImVec2(90, 50)) || (enterPressed && ImGui::IsKeyDown(ImGuiKey_ModCtrl))) {
        if (strlen(inputBuffer_) > 0) {
            std::string message = inputBuffer_;
            addMessage("You", message, Message::User);
            
            // Send to controller
            if (controller_) {
                controller_->sendMessage(message.c_str());
            }
            
            // Clear input
            memset(inputBuffer_, 0, sizeof(inputBuffer_));
        }
    }
    
    ImGui::Spacing();
}

void ChatWindow::addMessage(const std::string& sender, const std::string& content, Message::Type type) {
    messages_.push_back({sender, content, type});
    scrollToBottom_ = true;
}

void ChatWindow::clear() {
    messages_.clear();
}
