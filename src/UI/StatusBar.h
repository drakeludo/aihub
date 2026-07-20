#pragma once

#include "../Core/PerformanceMonitor.h"
#include "../Services/ThemeService.h"
#include <imgui.h>
#include <Windows.h>
#include <Psapi.h>

class StatusBar {
public:
    StatusBar() {}
    
    void render() {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 pos = viewport->WorkPos;
        pos.y += viewport->WorkSize.y - 30;
        
        ImGui::SetNextWindowPos(pos);
        ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, 30));
        
        ImGuiWindowFlags flags = 
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoDocking;
        
        const auto& colors = ThemeService::instance().getColors();
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.15f, 0.95f));
        
        ImGui::Begin("##StatusBar", nullptr, flags);
        
        // FPS
        float fps = PerformanceMonitor::instance().getFPS();
        ImGui::Text("FPS: %.1f", fps);
        
        // Frame time
        ImGui::SameLine(120);
        float frameTime = PerformanceMonitor::instance().getAverageFrameTime();
        ImGui::Text("%.2f ms", frameTime);
        
        // Memory
        ImGui::SameLine(240);
        PROCESS_MEMORY_COUNTERS pmc;
        if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
            float memMB = pmc.WorkingSetSize / (1024.0f * 1024.0f);
            ImGui::Text("Memory: %.1f MB", memMB);
        }
        
        // Theme
        ImGui::SameLine(ImGui::GetWindowWidth() - 150);
        const char* themeName = "Unknown";
        switch (ThemeService::instance().getCurrentTheme()) {
            case Theme::Cyber: themeName = "Cyber"; break;
            case Theme::Neon: themeName = "Neon"; break;
            case Theme::Matrix: themeName = "Matrix"; break;
            case Theme::Synthwave: themeName = "Synthwave"; break;
            case Theme::TechLab: themeName = "Tech Lab"; break;
        }
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 1.0f, 1.0f), "Theme: %s", themeName);
        
        ImGui::End();
        ImGui::PopStyleColor();
    }
};
