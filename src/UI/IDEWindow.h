#pragma once

#include <string>
#include <vector>

class IDEWindow {
public:
    IDEWindow();
    void render(bool* p_open);
    
    // File operations
    void newFile();
    void openFile(const std::string& path);
    void saveFile();
    void saveFileAs(const std::string& path);
    
    // Code editing
    std::string getCurrentCode() const;
    void setCode(const std::string& code);
    
private:
    char codeBuffer_[1024 * 100]; // 100KB buffer for code
    std::string currentFile_;
    bool hasUnsavedChanges_{false};
    
    // UI helpers
    void renderMenuBar();
    void renderEditor();
    void renderStatusBar();
};
