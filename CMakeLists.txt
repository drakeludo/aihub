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
    src/Core/Event.h
    src/Core/EventBus.h
    src/Core/ThreadPool.h
    src/Core/PerformanceMonitor.h
    
    # Models
    src/Models/ChatMessage.h
    src/Models/Provider.h
    src/Models/Conversation.h
    src/Models/UserSettings.h
    
    # Services
    src/Services/ChatService.cpp
    src/Services/ChatService.h
    src/Services/ThemeService.cpp
    src/Services/ThemeService.h
    src/Services/HistoryService.cpp
    src/Services/HistoryService.h
    
    # Logger
    src/Logger/Logger.cpp
    src/Logger/Logger.h
    
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
    src/UI/StatusBar.h
    src/UI/Animation.h
    src/UI/ParticleSystem.h
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

# Compiler options
if(MSVC)
    target_compile_options(AIHub PRIVATE /W4 /permissive- /utf-8)
    target_compile_definitions(AIHub PRIVATE 
        UNICODE 
        _UNICODE
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
    )
    set_target_properties(AIHub PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

# Install
install(TARGETS AIHub RUNTIME DESTINATION bin)

message(STATUS "================================")
message(STATUS "AIHub - ImGui + DirectX 11")
message(STATUS "  Modern C++23")
message(STATUS "  MIT License")
message(STATUS "================================")
