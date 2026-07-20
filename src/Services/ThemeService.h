#pragma once

#include "../Models/UserSettings.h"
#include <string>
#include <map>

struct ThemeColors {
    unsigned int background;
    unsigned int panel;
    unsigned int text;
    unsigned int textDim;
    unsigned int accent;
    unsigned int accentHover;
    unsigned int success;
    unsigned int warning;
    unsigned int error;
    unsigned int glow;
    
    // Cyberpunk specific
    unsigned int neon1;
    unsigned int neon2;
    unsigned int neon3;
};

class ThemeService {
public:
    static ThemeService& instance() {
        static ThemeService instance;
        return instance;
    }
    
    void setTheme(Theme theme);
    Theme getCurrentTheme() const { return currentTheme_; }
    const ThemeColors& getColors() const { return colors_; }
    
    void applyToImGui();
    
private:
    ThemeService();
    ~ThemeService() = default;
    ThemeService(const ThemeService&) = delete;
    ThemeService& operator=(const ThemeService&) = delete;
    
    void loadTheme(Theme theme);
    void loadCyberTheme();
    void loadNeonTheme();
    void loadMatrixTheme();
    void loadSynthwaveTheme();
    void loadTechLabTheme();
    
    Theme currentTheme_{Theme::Cyber};
    ThemeColors colors_;
};
