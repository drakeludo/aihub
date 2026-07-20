cmake_minimum_required(VERSION 3.25)
project(AIHub VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

# Windows only
if(NOT WIN32)
    message(FATAL_ERROR "This project is Windows-only")
endif()

# Find packages
find_package(imgui CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

# Application source files
set(SOURCES
    src/main_imgui.cpp
    src/Application.cpp
    src/Application.h
    
    # Core
    src/Core/AIRequest.h
    src/Core/IBrowser.h
    src/Core/BrowserFactory.cpp
    src/Core/BrowserFactory.h
    
    # Browser (WebView2 based)
    src/Browser/BrowserManager.cpp
    src/Browser/BrowserManager.h
    src/Browser/WebView2Instance.cpp
    src/Browser/WebView2Instance.h
    src/Browser/DeepSeekBrowser.cpp
    src/Browser/DeepSeekBrowser.h
    src/Browser/ChatGPTBrowser.cpp
    src/Browser/ChatGPTBrowser.h
    src/Browser/KiroBrowser.cpp
    src/Browser/KiroBrowser.h
    
    # Controllers
    src/Controllers/ChatController.cpp
    src/Controllers/ChatController.h
    
    # Services
    src/Services/AIService.cpp
    src/Services/AIService.h
    
    # UI (ImGui based)
    src/UI/ImGuiTheme.cpp
    src/UI/ImGuiTheme.h
    src/UI/ChatWindow.cpp
    src/UI/ChatWindow.h
    src/UI/SidebarWindow.cpp
    src/UI/SidebarWindow.h
    src/UI/LogWindow.cpp
    src/UI/LogWindow.h
    src/UI/SettingsWindow.cpp
    src/UI/SettingsWindow.h
    
    # Logger
    src/Logger/Logger.cpp
    src/Logger/Logger.h
    
    # Utils
    src/Utils/Database.h
    src/Utils/WorkspaceManager.h
)

# Executable
add_executable(AIHub WIN32 ${SOURCES})

# Include directories
target_include_directories(AIHub PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(AIHub PRIVATE
    imgui::imgui
    spdlog::spdlog
    d3d11
    dxgi
    d3dcompiler
    windowscodecs
)

# WebView2 (optional - will add later)
# For now, we'll implement basic browser automation

# Compiler options
if(MSVC)
    target_compile_options(AIHub PRIVATE /W4 /permissive- /utf-8)
    target_compile_definitions(AIHub PRIVATE 
        UNICODE 
        _UNICODE
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
    )
    # Set subsystem to Windows (no console)
    set_target_properties(AIHub PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

# Install
install(TARGETS AIHub RUNTIME DESTINATION bin)

message(STATUS "================================")
message(STATUS "AIHub - ImGui + WebView2 Version")
message(STATUS "  No Qt required!")
message(STATUS "  100% Free (MIT License)")
message(STATUS "  Native Windows (DirectX 11)")
message(STATUS "================================")
