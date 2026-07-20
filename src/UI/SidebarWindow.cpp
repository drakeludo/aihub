#include "SidebarWindow.h"
#include "Controllers/ChatController.h"
#include "imgui.h"

SidebarWindow::SidebarWindow(ChatController* controller)
    : controller_(controller)
{
}

void SidebarWindow::render() {
    ImGui::Begin("Sidebar", nullptr, ImGuiWindowFlags_NoCollapse);
    
    // Header
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.72f, 0.30f, 1.0f));
    ImGui::TextWrapped("AI Hub");
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Spacing();
    
    // New Chat button
    if (ImGui::Button("+ New Chat", ImVec2(-1, 40))) {
        if (controller_) {
            controller_->createNewChat();
        }
    }
    
    ImGui::Spacing();
    
    // Provider selection
    ImGui::Text("AI Provider:");
    const char* providers[] = { "Auto", "DeepSeek", "ChatGPT", "Kiro" };
    static int currentProvider = 0;
    ImGui::Combo("##provider", &currentProvider, providers, IM_ARRAYSIZE(providers));
    
    ImGui::Spacing();
    
    // Strategy selection
    ImGui::Text("Strategy:");
    const char* strategies[] = { "Single", "Chain (DS→GPT)", "Parallel", "Best" };
    static int currentStrategy = 1;
    ImGui::Combo("##strategy", &currentStrategy, strategies, IM_ARRAYSIZE(strategies));
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Chat list
    ImGui::Text("Chats:");
    ImGui::BeginChild("ChatList", ImVec2(0, -50), true);
    
    // TODO: Render chat list from database
    ImGui::Text("Chat 1");
    ImGui::Text("Chat 2");
    
    ImGui::EndChild();
    
    ImGui::Spacing();
    
    // Settings button
    if (ImGui::Button("⚙ Settings", ImVec2(-1, 36))) {
        // Open settings
    }
    
    ImGui::End();
}
