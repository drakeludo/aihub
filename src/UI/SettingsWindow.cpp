#include "SettingsWindow.h"
#include "imgui.h"

void SettingsWindow::render(bool* open) {
    ImGui::Begin("Settings", open);
    static bool autoSaveConversations = true;
    
    if (ImGui::BeginTabBar("SettingsTabs")) {
        if (ImGui::BeginTabItem("General")) {
            ImGui::Text("Default Provider:");
            const char* providers[] = { "Auto", "DeepSeek", "ChatGPT", "Kiro" };
            static int provider = 0;
            ImGui::Combo("##defprov", &provider, providers, IM_ARRAYSIZE(providers));
            
            ImGui::Spacing();
            ImGui::Checkbox("Auto-save conversations", &autoSaveConversations);
            
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Appearance")) {
            ImGui::Text("Theme:");
            const char* themes[] = { "Dark", "Light", "Purple", "OLED" };
            static int theme = 0;
            ImGui::Combo("##theme", &theme, themes, IM_ARRAYSIZE(themes));
            
            ImGui::EndTabItem();
        }
        
        ImGui::EndTabBar();
    }
    
    ImGui::End();
}
