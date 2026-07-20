#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <memory>

class ChatController;
class ChatWindow;
class SidebarWindow;
class LogWindow;
class SettingsWindow;

class Application {
public:
    Application();
    ~Application();
    
    bool initialize();
    int run();
    void shutdown();
    
    // Window procedure
    static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    
private:
    bool createDeviceD3D();
    void cleanupDeviceD3D();
    void createImGuiContext();
    void cleanupImGuiContext();
    
    void newFrame();
    void render();
    void renderUI();
    
    // Window
    HWND hwnd_{nullptr};
    WNDCLASSEXW wc_{};
    
    // DirectX 11
    ID3D11Device* d3dDevice_{nullptr};
    ID3D11DeviceContext* d3dDeviceContext_{nullptr};
    IDXGISwapChain* swapChain_{nullptr};
    ID3D11RenderTargetView* mainRenderTargetView_{nullptr};
    
    // Application state
    bool showDemoWindow_{false};
    bool showSettingsWindow_{false};
    bool showLogWindow_{true};
    
    // Components
    std::unique_ptr<ChatController> chatController_;
    std::unique_ptr<ChatWindow> chatWindow_;
    std::unique_ptr<SidebarWindow> sidebarWindow_;
    std::unique_ptr<LogWindow> logWindow_;
    std::unique_ptr<SettingsWindow> settingsWindow_;
    
    // Window size
    int width_{1400};
    int height_{900};
};
