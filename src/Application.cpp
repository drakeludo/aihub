#include "Application.h"
#include "UI/ChatWindow.h"
#include "UI/SidebarWindow.h"
#include "UI/LogWindow.h"
#include "UI/SettingsWindow.h"
#include "UI/StatusBar.h"
#include "UI/ParticleSystem.h"
#include "UI/ImGuiTheme.h"
#include "Logger/Logger.h"
#include "Services/ChatService.h"
#include "Services/ThemeService.h"
#include "Core/PerformanceMonitor.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include <tchar.h>
#include <filesystem>

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Application::Application() {
}

Application::~Application() {
}

bool Application::initialize() {
    Logger::instance().info("Creating window class...");
    
    // Create window class
    wc_ = { sizeof(wc_), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"AIHub", nullptr };
    ::RegisterClassExW(&wc_);
    
    Logger::instance().info("Creating window...");
    
    // Create window
    hwnd_ = ::CreateWindowW(wc_.lpszClassName, L"AI Hub", WS_OVERLAPPEDWINDOW, 
                            100, 100, width_, height_, nullptr, nullptr, wc_.hInstance, nullptr);
    if (!hwnd_) {
        Logger::instance().error("Failed to create window");
        return false;
    }
    
    Logger::instance().info("Window created successfully");
    
    // Store this pointer
    ::SetWindowLongPtr(hwnd_, GWLP_USERDATA, (LONG_PTR)this);
    
    Logger::instance().info("Initializing Direct3D...");
    
    // Initialize Direct3D
    if (!createDeviceD3D()) {
        Logger::instance().error("Failed to create D3D device");
        cleanupDeviceD3D();
        ::UnregisterClassW(wc_.lpszClassName, wc_.hInstance);
        return false;
    }
    
    Logger::instance().info("Direct3D initialized successfully");
    
    // Show window
    ::ShowWindow(hwnd_, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd_);
    
    // Setup ImGui context
    try {
        createImGuiContext();
        Logger::instance().info("ImGui context created successfully");
    } catch (const std::exception& e) {
        Logger::instance().error("Failed to create ImGui context: {}", e.what());
        return false;
    } catch (...) {
        Logger::instance().error("Failed to create ImGui context: Unknown error");
        return false;
    }
    
    // Initialize services
    Logger::instance().info("Initializing services...");
    ThemeService::instance().applyToImGui();
    ChatService::instance().createConversation("ChatGPT", "gpt-4");
    
    Logger::instance().info("Creating UI components...");
    
    // Initialize components
    chatWindow_ = std::make_unique<ChatWindow>();
    sidebarWindow_ = std::make_unique<SidebarWindow>();
    logWindow_ = std::make_unique<LogWindow>();
    settingsWindow_ = std::make_unique<SettingsWindow>();
    statusBar_ = std::make_unique<StatusBar>();
    particles_ = std::make_unique<ParticleSystem>(200);
    
    Logger::instance().info("Application initialized successfully");
    
    return true;
}

int Application::run() {
    // Main loop
    bool done = false;
    while (!done) {
        PerformanceMonitor::instance().beginFrame();
        
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
        
        PerformanceMonitor::instance().endFrame();
    }
    
    return 0;
}

void Application::shutdown() {
    // Cleanup
    particles_.reset();
    statusBar_.reset();
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
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    
    HRESULT res = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, 
        featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &swapChain_, 
        &d3dDevice_, &featureLevel, &d3dDeviceContext_
    );
    
    if (res != S_OK || !d3dDevice_ || !swapChain_) {
        Logger::instance().error("Failed to create D3D11 device and swap chain. HRESULT: {}", static_cast<int>(res));
        return false;
    }
    
    // Create render target
    ID3D11Texture2D* pBackBuffer = nullptr;
    swapChain_->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (!pBackBuffer) {
        Logger::instance().error("Failed to get back buffer");
        return false;
    }
    
    d3dDevice_->CreateRenderTargetView(pBackBuffer, nullptr, &mainRenderTargetView_);
    pBackBuffer->Release();
    
    if (!mainRenderTargetView_) {
        Logger::instance().error("Failed to create render target view");
        return false;
    }
    
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
    
    Logger::instance().info("ImGui context created, setting up style...");
    
    // Setup Dear ImGui style
    ImGuiTheme::ApplyDarkTheme();
    
    Logger::instance().info("Initializing ImGui backends...");
    
    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd_);
    ImGui_ImplDX11_Init(d3dDevice_, d3dDeviceContext_);
    
    Logger::instance().info("Loading fonts...");
    
    // Load fonts (with fallback to default)
    const char* fontPaths[] = {
        "c:\\Windows\\Fonts\\segoeui.ttf",
        "c:\\Windows\\Fonts\\arial.ttf",
        "c:\\Windows\\Fonts\\Arial.ttf"
    };
    
    bool fontLoaded = false;
    for (const char* path : fontPaths) {
        try {
            if (std::filesystem::exists(path)) {
                ImFont* font = io.Fonts->AddFontFromFileTTF(path, 16.0f);
                if (font) {
                    fontLoaded = true;
                    Logger::instance().info("Loaded font: {}", path);
                    break;
                }
            }
        } catch (...) {
            Logger::instance().warning("Failed to load font: {}", path);
        }
    }
    
    if (!fontLoaded) {
        io.Fonts->AddFontDefault();
        Logger::instance().warning("Using default ImGui font (no system fonts found)");
    }
    
    Logger::instance().info("Font loading complete");
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

    swapChain_->Present(1, 0); // Present with vsync
}

void Application::renderUI() {
    // Background effects
    renderBackground();

    if (ImGui::BeginMainMenuBar()) {
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
        
        ImGui::EndMainMenuBar();
    }

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
    
    // Status bar
    statusBar_->render();
    
    // Glow effects
    renderGlowEffects();
}

void Application::renderBackground() {
    backgroundTime_ += 0.016f;
    
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    
    const auto& colors = ThemeService::instance().getColors();
    
    // Animated gradient overlay
    float pulse = (std::sin(backgroundTime_ * 0.5f) + 1.0f) * 0.5f;
    ImU32 gradientColor = IM_COL32(
        ((colors.neon1 >> 24) & 0xFF) * pulse * 0.1f,
        ((colors.neon1 >> 16) & 0xFF) * pulse * 0.1f,
        ((colors.neon1 >> 8) & 0xFF) * pulse * 0.1f,
        20
    );
    
    drawList->AddRectFilledMultiColor(
        viewport->WorkPos,
        ImVec2(viewport->WorkPos.x + viewport->WorkSize.x, viewport->WorkPos.y + viewport->WorkSize.y),
        gradientColor,
        IM_COL32(0, 0, 0, 0),
        gradientColor,
        IM_COL32(0, 0, 0, 0)
    );
    
    // Particles
    particles_->update(0.016f);
    particles_->render(drawList);
    
    // Emit particles occasionally
    if (static_cast<int>(backgroundTime_ * 2.0f) % 10 == 0 && particles_->getParticleCount() < 50) {
        ImVec2 emitPos(
            viewport->WorkPos.x + std::rand() % static_cast<int>(viewport->WorkSize.x),
            viewport->WorkPos.y + std::rand() % static_cast<int>(viewport->WorkSize.y)
        );
        ImVec4 color(
            ((colors.neon2 >> 24) & 0xFF) / 255.0f,
            ((colors.neon2 >> 16) & 0xFF) / 255.0f,
            ((colors.neon2 >> 8) & 0xFF) / 255.0f,
            0.5f
        );
        particles_->emit(emitPos, color, 1.5f);
    }
}

void Application::renderGlowEffects() {
    // TODO: Add bloom/glow post-processing
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
