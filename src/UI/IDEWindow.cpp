#include "IDEWindow.h"
#include <imgui.h>
#include <fstream>
#include <sstream>

IDEWindow::IDEWindow() {
    memset(codeBuffer_, 0, sizeof(codeBuffer_));
    strcpy_s(codeBuffer_, sizeof(codeBuffer_), "// Your code here\n");
}

void IDEWindow::render(bool* p_open) {
    // Claude Code IDE style window
    ImGui::SetNextWindowSize(ImVec2(1000, 700), ImGuiCond_FirstUseEver);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.11f, 0.11f, 0.11f, 1.0f)); // #1C1C1C
    
    if (!ImGui::Begin("IDE - Claude Code Style", p_open, ImGuiWindowFlags_MenuBar)) {
        ImGui::End();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        return;
    }
    
    renderMenuBar();
    renderEditor();
    renderStatusBar();
    
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}

void IDEWindow::renderMenuBar() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New File", "Ctrl+N")) {
                newFile();
            }
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
                // TODO: File dialog
            }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                saveFile();
            }
            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
                // TODO: File dialog
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Close", "Ctrl+W")) {
                // Will close via p_open
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z")) {}
            if (ImGui::MenuItem("Redo", "Ctrl+Y")) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "Ctrl+X")) {}
            if (ImGui::MenuItem("Copy", "Ctrl+C")) {}
            if (ImGui::MenuItem("Paste", "Ctrl+V")) {}
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Word Wrap")) {}
            if (ImGui::MenuItem("Line Numbers")) {}
            ImGui::EndMenu();
        }
        
        ImGui::EndMenuBar();
    }
}

void IDEWindow::renderEditor() {
    // Claude Code style editor
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.09f, 0.09f, 0.09f, 1.0f)); // #171717
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
    
    ImVec2 editorSize = ImVec2(
        ImGui::GetWindowWidth(),
        ImGui::GetWindowHeight() - 75 // Leave space for menu and status bar
    );
    
    ImGui::InputTextMultiline(
        "##code",
        codeBuffer_,
        sizeof(codeBuffer_),
        editorSize,
        ImGuiInputTextFlags_AllowTabInput
    );
    
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);
}

void IDEWindow::renderStatusBar() {
    // Bottom status bar (Claude Code style)
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.09f, 0.09f, 0.09f, 1.0f));
    ImGui::BeginChild("StatusBar", ImVec2(0, 25), false, ImGuiWindowFlags_NoScrollbar);
    
    ImGui::SetCursorPosY(4);
    ImGui::SetCursorPosX(10);
    
    // File name
    if (currentFile_.empty()) {
        ImGui::Text("Untitled");
    } else {
        ImGui::Text("%s", currentFile_.c_str());
    }
    
    // Modified indicator
    if (hasUnsavedChanges_) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.0f, 1.0f), " *");
    }
    
    // Language/encoding
    ImGui::SameLine(ImGui::GetWindowWidth() - 200);
    ImGui::TextDisabled("Plain Text | UTF-8");
    
    ImGui::EndChild();
    ImGui::PopStyleColor();
}

void IDEWindow::newFile() {
    currentFile_ = "";
    memset(codeBuffer_, 0, sizeof(codeBuffer_));
    strcpy_s(codeBuffer_, sizeof(codeBuffer_), "// Your code here\n");
    hasUnsavedChanges_ = false;
}

void IDEWindow::openFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return;
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    if (content.size() < sizeof(codeBuffer_)) {
        strcpy_s(codeBuffer_, sizeof(codeBuffer_), content.c_str());
        currentFile_ = path;
        hasUnsavedChanges_ = false;
    }
}

void IDEWindow::saveFile() {
    if (currentFile_.empty()) {
        // TODO: Show save dialog
        return;
    }
    
    std::ofstream file(currentFile_);
    if (file.is_open()) {
        file << codeBuffer_;
        file.close();
        hasUnsavedChanges_ = false;
    }
}

void IDEWindow::saveFileAs(const std::string& path) {
    currentFile_ = path;
    saveFile();
}

std::string IDEWindow::getCurrentCode() const {
    return std::string(codeBuffer_);
}

void IDEWindow::setCode(const std::string& code) {
    if (code.size() < sizeof(codeBuffer_)) {
        strcpy_s(codeBuffer_, sizeof(codeBuffer_), code.c_str());
        hasUnsavedChanges_ = true;
    }
}
