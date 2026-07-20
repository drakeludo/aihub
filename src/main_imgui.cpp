// AI Hub - ImGui + DirectX 11 + WebView2 Version
// 100% Free & Open Source (MIT License)

#include "Application.h"
#include "Logger/Logger.h"
#include <Windows.h>

int main(int argc, char** argv) {
    // Initialize logger
    Logger::instance().initialize();
    Logger::instance().info("AI Hub starting (ImGui version)...");
    
    // Create application
    Application app;
    
    if (!app.initialize()) {
        Logger::instance().error("Failed to initialize application");
        MessageBoxA(nullptr, "Failed to initialize application", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    // Run application
    int result = app.run();
    
    // Cleanup
    app.shutdown();
    
    Logger::instance().info("AI Hub shutting down");
    
    return result;
}

// Windows entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    return main(__argc, __argv);
}
