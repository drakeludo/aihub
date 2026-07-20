#include "ThemeService.h"
#include "../Core/EventBus.h"
#include "../Logger/Logger.h"
#include <imgui.h>

ThemeService::ThemeService() {
    loadTheme(Theme::Cyber);
}

void ThemeService::setTheme(Theme theme) {
    if (currentTheme_ == theme) return;
    
    currentTheme_ = theme;
    loadTheme(theme);
    applyToImGui();
    
    Logger::instance().info("Theme changed");
    EventBus::instance().emit(EventType::ThemeChanged);
}

void ThemeService::loadTheme(Theme theme) {
    switch (theme) {
        case Theme::Cyber:      loadCyberTheme(); break;
        case Theme::Neon:       loadNeonTheme(); break;
        case Theme::Matrix:     loadMatrixTheme(); break;
        case Theme::Synthwave:  loadSynthwaveTheme(); break;
        case Theme::TechLab:    loadTechLabTheme(); break;
    }
}

void ThemeService::loadCyberTheme() {
    // Dark cyberpunk theme
    colors_.background   = 0xFF0A0A0F;  // Very dark
    colors_.panel        = 0xFF1A1A2E;  // Dark blue-grey
    colors_.text         = 0xFFE0E0E0;  // Light grey
    colors_.textDim      = 0xFF808080;  // Dim grey
    colors_.accent       = 0xFFFF006B;  // Neon pink
    colors_.accentHover  = 0xFFFF4D8F;  // Lighter pink
    colors_.success      = 0xFF00FF9F;  // Neon green
    colors_.warning      = 0xFFFFAA00;  // Orange
    colors_.error        = 0xFFFF3333;  // Red
    colors_.glow         = 0x80FF006B;  // Pink glow (semi-transparent)
    
    colors_.neon1        = 0xFFFF006B;  // Pink
    colors_.neon2        = 0xFF00F0FF;  // Cyan
    colors_.neon3        = 0xFFBB00FF;  // Purple
}

void ThemeService::loadNeonTheme() {
    colors_.background   = 0xFF000000;
    colors_.panel        = 0xFF0D0D1F;
    colors_.text         = 0xFFF0F0FF;
    colors_.textDim      = 0xFF909099;
    colors_.accent       = 0xFF00FFFF;  // Cyan
    colors_.accentHover  = 0xFF33FFFF;
    colors_.success      = 0xFF00FF00;
    colors_.warning      = 0xFFFFFF00;
    colors_.error        = 0xFFFF0055;
    colors_.glow         = 0x8000FFFF;
    
    colors_.neon1        = 0xFF00FFFF;
    colors_.neon2        = 0xFFFF00FF;
    colors_.neon3        = 0xFFFFFF00;
}

void ThemeService::loadMatrixTheme() {
    colors_.background   = 0xFF000000;
    colors_.panel        = 0xFF001100;
    colors_.text         = 0xFF00FF00;  // Matrix green
    colors_.textDim      = 0xFF008800;
    colors_.accent       = 0xFF00FF00;
    colors_.accentHover  = 0xFF33FF33;
    colors_.success      = 0xFF00FF00;
    colors_.warning      = 0xFFFFFF00;
    colors_.error        = 0xFFFF0000;
    colors_.glow         = 0x8000FF00;
    
    colors_.neon1        = 0xFF00FF00;
    colors_.neon2        = 0xFF00DD00;
    colors_.neon3        = 0xFF00BB00;
}

void ThemeService::loadSynthwaveTheme() {
    colors_.background   = 0xFF0A0515;
    colors_.panel        = 0xFF1A1535;
    colors_.text         = 0xFFFFE0FF;
    colors_.textDim      = 0xFF9080A0;
    colors_.accent       = 0xFFFF00FF;  // Magenta
    colors_.accentHover  = 0xFFFF66FF;
    colors_.success      = 0xFF00FFAA;
    colors_.warning      = 0xFFFFAA00;
    colors_.error        = 0xFFFF0080;
    colors_.glow         = 0x80FF00FF;
    
    colors_.neon1        = 0xFFFF00FF;
    colors_.neon2        = 0xFF00FFFF;
    colors_.neon3        = 0xFFFF6600;
}

void ThemeService::loadTechLabTheme() {
    colors_.background   = 0xFF0F0F15;
    colors_.panel        = 0xFF1F1F2A;
    colors_.text         = 0xFFDDDDFF;
    colors_.textDim      = 0xFF7777AA;
    colors_.accent       = 0xFF4488FF;  // Blue
    colors_.accentHover  = 0xFF66AAFF;
    colors_.success      = 0xFF00DD88;
    colors_.warning      = 0xFFFFBB44;
    colors_.error        = 0xFFFF4444;
    colors_.glow         = 0x804488FF;
    
    colors_.neon1        = 0xFF4488FF;
    colors_.neon2        = 0xFF00DDAA;
    colors_.neon3        = 0xFFFF8844;
}

void ThemeService::applyToImGui() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;
    
    auto toVec4 = [](unsigned int rgba) {
        return ImVec4(
            ((rgba >> 24) & 0xFF) / 255.0f,
            ((rgba >> 16) & 0xFF) / 255.0f,
            ((rgba >> 8) & 0xFF) / 255.0f,
            (rgba & 0xFF) / 255.0f
        );
    };
    
    colors[ImGuiCol_WindowBg] = toVec4(colors_.background);
    colors[ImGuiCol_ChildBg] = toVec4(colors_.panel);
    colors[ImGuiCol_Text] = toVec4(colors_.text);
    colors[ImGuiCol_TextDisabled] = toVec4(colors_.textDim);
    colors[ImGuiCol_Button] = toVec4(colors_.accent);
    colors[ImGuiCol_ButtonHovered] = toVec4(colors_.accentHover);
    colors[ImGuiCol_FrameBg] = toVec4(colors_.panel);
    colors[ImGuiCol_Header] = toVec4(colors_.accent);
    colors[ImGuiCol_HeaderHovered] = toVec4(colors_.accentHover);
    
    // Rounding
    style.WindowRounding = 8.0f;
    style.ChildRounding = 6.0f;
    style.FrameRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.PopupRounding = 6.0f;
    style.ScrollbarRounding = 4.0f;
    style.TabRounding = 4.0f;
    
    // Spacing
    style.WindowPadding = ImVec2(12.0f, 12.0f);
    style.FramePadding = ImVec2(8.0f, 4.0f);
    style.ItemSpacing = ImVec2(8.0f, 6.0f);
    style.ItemInnerSpacing = ImVec2(6.0f, 4.0f);
}
