#include "SidebarWindow.h"
#include "../Services/ChatService.h"
#include "../Services/ThemeService.h"
#include <imgui.h>

SidebarWindow::SidebarWindow() {
}

void SidebarWindow::render() {
    ImGui::Begin("Sidebar", nullptr, ImGuiWindowFlags_NoCollapse);
    
    // Header
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.72f, 0.30f, 1.0f));
    ImGui::TextWrapped("🚀 AI Hub");
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Spacing();
    
    // New Chat button
    if (ImGui::Button("+ New Chat", ImVec2(-1, 40))) {
        ChatService::instance().createConversation("ChatGPT", "gpt-4");
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Chat list
    ImGui::Text("Conversations:");
    ImGui::BeginChild("ChatList", ImVec2(0, -50), true);
    
    auto conversations = ChatService::instance().getConversations();
    for (size_t i = 0; i < conversations.size(); i++) {
        const auto& conv = conversations[i];
        
        bool isSelected = (selectedChat_ == static_cast<int>(i));
        if (ImGui::Selectable(("💬 " + conv.title).c_str(), isSelected)) {
            selectedChat_ = static_cast<int>(i);
            ChatService::instance().loadConversation(conv.id);
        }
        
        // Show message count
        if (!conv.messages.empty()) {
            ImGui::SameLine(ImGui::GetWindowWidth() - 60);
            ImGui::TextDisabled("(%d)", static_cast<int>(conv.messages.size()));
        }
    }
    
    if (conversations.empty()) {
        ImGui::TextDisabled("No conversations yet");
    }
    
    ImGui::EndChild();
    
    ImGui::Spacing();
    
    // Settings button
    if (ImGui::Button("⚙️ Settings", ImVec2(-1, 36))) {
        // TODO: Open settings
    }
    
    ImGui::End();
}
