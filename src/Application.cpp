#include "Application.h"
#include "UI/ChatWindow.h"
#include "UI/SidebarWindow.h"
#include "UI/LogWindow.h"
#include "UI/SettingsWindow.h"
#include "UI/ImGuiTheme.h"
#include "Logger/Logger.h"
#include "Services/ChatService.h"
#include "Services/ThemeService.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include <tchar.h>

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Application::Application() {
}

Application::~Application() {
}

bool Application::initialize() {
    // Create window class
    wc_ = { sizeof(wc_), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"AIHub", nullptr };
    ::RegisterClassExW(&wc_);
    
    // Create window
    hwnd_ = ::CreateWindowW(wc_.lpszClassName, L"AI Hub", WS_OVERLAPPEDWINDOW, 
                            100, 100, width_, height_, nullptr, nullptr, wc_.hInstance, nullptr);
    if (!hwnd_) {
        Logger::instance().error("Failed to create window");
        return false;
    }
    
    // Store this pointer
    ::SetWindowLongPtr(hwnd_, GWLP_USERDATA, (LONG_PTR)this);
    
    // Initialize Direct3D
    if (!createDeviceD3D()) {
        cleanupDeviceD3D();
        ::UnregisterClassW(wc_.lpszClassName, wc_.hInstance);
        return false;
    }
    
    // Show window
    ::ShowWindow(hwnd_, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd_);
    
    // Setup ImGui context
    createImGuiContext();
    
    // Initialize services
    ThemeService::instance().applyToImGui();
    ChatService::instance().createConversation("ChatGPT", "gpt-4");
    
    // Initialize components
    chatWindow_ = std::make_unique<ChatWindow>();
    sidebarWindow_ = std::make_unique<SidebarWindow>();
    logWindow_ = std::make_unique<LogWindow>();
    settingsWindow_ = std::make_unique<SettingsWindow>();
    
    Logger::instance().info("Application initialized successfully");
    
    return true;
}

int Application::run() {
    // Main loop
    bool done = false;
    while (!done) {
        // Poll and handle messages
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;
        
        // Start ImGui frame
        newFrame();
        
        // Render UI
        renderUI();
        
        // Rendering
        render();
    }
    
    return 0;
}

void Application::shutdown() {
    // Cleanup
    chatWindow_.reset();
    sidebarWindow_.reset();
    logWindow_.reset();
    settingsWindow_.reset();
    
    cleanupImGuiContext();
    cleanupDeviceD3D();
    
    ::DestroyWindow(hwnd_);
    ::UnregisterClassW(wc_.lpszClassName, wc_.hInstance);
}

bool Application::createDeviceD3D() {
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd_;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    
    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    
    HRESULT res = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, 
        featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &swapChain_, 
        &d3dDevice_, &featureLevel, &d3dDeviceContext_
    );
    
    if (res != S_OK) {
        Logger::instance().error("Failed to create D3D11 device and swap chain");
        return false;
    }
    
    // Create render target
    ID3D11Texture2D* pBackBuffer;
    swapChain_->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    d3dDevice_->CreateRenderTargetView(pBackBuffer, nullptr, &mainRenderTargetView_);
    pBackBuffer->Release();
    
    return true;
}

void Application::cleanupDeviceD3D() {
    if (mainRenderTargetView_) { mainRenderTargetView_->Release(); mainRenderTargetView_ = nullptr; }
    if (swapChain_) { swapChain_->Release(); swapChain_ = nullptr; }
    if (d3dDeviceContext_) { d3dDeviceContext_->Release(); d3dDeviceContext_ = nullptr; }
    if (d3dDevice_) { d3dDevice_->Release(); d3dDevice_ = nullptr; }
}

void Application::createImGuiContext() {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    
    // Setup Dear ImGui style
    ImGuiTheme::ApplyDarkTheme();
    
    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    
    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd_);
    ImGui_ImplDX11_Init(d3dDevice_, d3dDeviceContext_);
    
    // Load fonts
    io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 16.0f);
}

void Application::cleanupImGuiContext() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void Application::newFrame() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void Application::render() {
    ImGui::Render();
    
    const float clear_color[4] = { 0.12f, 0.12f, 0.12f, 1.00f };
    d3dDeviceContext_->OMSetRenderTargets(1, &mainRenderTargetView_, nullptr);
    d3dDeviceContext_->ClearRenderTargetView(mainRenderTargetView_, clear_color);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    
    // Update and Render additional Platform Windows
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
    
    swapChain_->Present(1, 0); // Present with vsync
}

void Application::renderUI() {
    // Dockspace
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
                                     ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar;
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(3);
    
    // Menu bar
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Chat", "Ctrl+N")) {
                ChatService::instance().createConversation("ChatGPT", "gpt-4");
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                PostQuitMessage(0);
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Logs", nullptr, &showLogWindow_);
            ImGui::MenuItem("ImGui Demo", nullptr, &showDemoWindow_);
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Theme")) {
            if (ImGui::MenuItem("Cyber")) ThemeService::instance().setTheme(Theme::Cyber);
            if (ImGui::MenuItem("Neon")) ThemeService::instance().setTheme(Theme::Neon);
            if (ImGui::MenuItem("Matrix")) ThemeService::instance().setTheme(Theme::Matrix);
            if (ImGui::MenuItem("Synthwave")) ThemeService::instance().setTheme(Theme::Synthwave);
            if (ImGui::MenuItem("Tech Lab")) ThemeService::instance().setTheme(Theme::TechLab);
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Settings")) {
            if (ImGui::MenuItem("Preferences")) {
                showSettingsWindow_ = true;
            }
            ImGui::EndMenu();
        }
        
        ImGui::EndMenuBar();
    }
    
    // Dockspace
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    
    ImGui::End();
    
    // Windows
    sidebarWindow_->render();
    chatWindow_->render();
    
    if (showLogWindow_) {
        logWindow_->render(&showLogWindow_);
    }
    
    if (showSettingsWindow_) {
        settingsWindow_->render(&showSettingsWindow_);
    }
    
    if (showDemoWindow_) {
        ImGui::ShowDemoWindow(&showDemoWindow_);
    }
}

LRESULT WINAPI Application::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;
    
    Application* app = reinterpret_cast<Application*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
    
    switch (msg) {
        case WM_SIZE:
            if (app && app->d3dDevice_ != nullptr && wParam != SIZE_MINIMIZED) {
                app->cleanupDeviceD3D();
                app->swapChain_->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
                app->createDeviceD3D();
            }
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
