#include "LogWindow.h"
#include "imgui.h"

void LogWindow::render(bool* open) {
    ImGui::Begin("Logs", open);
    
    ImGui::TextColored(ImVec4(0.3f, 0.7f, 1.0f, 1.0f), "[INFO] AI Hub started");
    ImGui::TextColored(ImVec4(0.3f, 0.7f, 1.0f, 1.0f), "[INFO] ImGui version initialized");
    ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.3f, 1.0f), "[WARN] Qt version disabled");
    
    ImGui::End();
}
