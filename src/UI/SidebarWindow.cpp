#include "SidebarWindow.h"
#include "../Services/ChatService.h"
#include "../Services/ThemeService.h"
#include <imgui.h>

SidebarWindow::SidebarWindow() {
}

void SidebarWindow::render() {
    // Claude Code IDE style sidebar - minimal, clean
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.09f, 0.09f, 0.09f, 1.0f)); // #171717
    
    ImGui::Begin("Sidebar", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    
    // Header with logo
    ImGui::SetCursorPos(ImVec2(16, 16));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.5f, 0.9f, 1.0f)); // Purple
    ImGui::Text("AI Hub");
    ImGui::PopStyleColor();
    
    ImGui::SetCursorPosY(50);
    
    // New Chat button (Claude style)
    ImGui::SetCursorPosX(12);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.14f, 0.14f, 0.14f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.18f, 0.18f, 0.18f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.22f, 0.22f, 0.22f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 8));
    
    if (ImGui::Button("+ New Chat", ImVec2(ImGui::GetWindowWidth() - 24, 36))) {
        ChatService::instance().createConversation("ChatGPT", "gpt-4");
    }
    
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(3);
    
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 12);
    
    // Chat list
    ImGui::BeginChild("ChatList", ImVec2(0, -60), false, ImGuiWindowFlags_NoScrollbar);
    
    auto conversations = ChatService::instance().getConversations();
    
    ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0, 0.5f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2));
    
    for (size_t i = 0; i < conversations.size(); i++) {
        const auto& conv = conversations[i];
        
        bool isSelected = (selectedChat_ == static_cast<int>(i));
        
        ImGui::SetCursorPosX(8);
        
        // Claude style: highlight on hover, subtle selection
        if (isSelected) {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.18f, 0.18f, 0.18f, 1.0f));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0));
        }
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.16f, 0.16f, 0.16f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_SelectableRounding, 4.0f);
        
        if (ImGui::Selectable(("  " + conv.title).c_str(), isSelected, 0, ImVec2(ImGui::GetWindowWidth() - 16, 32))) {
            selectedChat_ = static_cast<int>(i);
            ChatService::instance().loadConversation(conv.id);
        }
        
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);
    }
    
    ImGui::PopStyleVar(2);
    
    if (conversations.empty()) {
        ImGui::SetCursorPos(ImVec2(20, 60));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
        ImGui::Text("No chats yet");
        ImGui::PopStyleColor();
    }
    
    ImGui::EndChild();
    
    // Bottom settings button
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    drawList->AddLine(ImVec2(p.x + 8, p.y), ImVec2(p.x + ImGui::GetWindowWidth() - 8, p.y), 
                     IM_COL32(40, 40, 40, 255), 1.0f);
    
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8);
    ImGui::SetCursorPosX(12);
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.16f, 0.16f, 0.16f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    
    if (ImGui::Button("Settings", ImVec2(ImGui::GetWindowWidth() - 24, 32))) {
        // TODO: Open settings
    }
    
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);
    
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}
