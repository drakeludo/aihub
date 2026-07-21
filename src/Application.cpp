#include "Application.h"
#include "UI/ChatWindow.h"
#include "UI/SidebarWindow.h"
#include "UI/LogWindow.h"
#include "UI/SettingsWindow.h"
#include "UI/StatusBar.h"
#include "UI/IDEWindow.h"
#include "UI/ImGuiTheme.h"
#include "Logger/Logger.h"
#include "Services/ChatService.h"
#include "Services/ThemeService.h"
#include "Services/BrowserService.h"
#include "Core/PerformanceMonitor.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include <tchar.h>
#include <filesystem>
#include <ctime>
#include <cstdlib>

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Application::Application() {
}

Application::~Application() {
}

bool Application::initialize() {
    // Initialize random seed for particles
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    
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
    ThemeService::instance().setTheme(Theme::Cyber); // Will apply Claude Code colors
    ChatService::instance().createConversation("ChatGPT", "gpt-4");
    
    Logger::instance().info("Creating UI components...");
    
    // Initialize components
    chatWindow_ = std::make_unique<ChatWindow>();
    sidebarWindow_ = std::make_unique<SidebarWindow>();
    logWindow_ = std::make_unique<LogWindow>();
    settingsWindow_ = std::make_unique<SettingsWindow>();
    statusBar_ = std::make_unique<StatusBar>();
    ideWindow_ = std::make_unique<IDEWindow>();
    
    // Initialize WebView2 for AI providers
    Logger::instance().info("Initializing BrowserService (WebView2)...");
    if (!BrowserService::instance().initialize(hwnd_)) {
        Logger::instance().error("Failed to initialize BrowserService");
        // Non-fatal: app can still work without WebView2
    }
    
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
    BrowserService::instance().shutdown();
    
    ideWindow_.reset();
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
    Logger::instance().info("Starting ImGui initialization...");
    
    try {
        // Setup Dear ImGui context
        Logger::instance().info("Calling IMGUI_CHECKVERSION...");
        IMGUI_CHECKVERSION();
        
        Logger::instance().info("Creating ImGui context...");
        ImGui::CreateContext();
        
        Logger::instance().info("Getting ImGui IO...");
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        
        Logger::instance().info("Applying theme...");
        ImGuiTheme::ApplyDarkTheme();
        
        Logger::instance().info("Initializing Win32 backend...");
        if (!ImGui_ImplWin32_Init(hwnd_)) {
            Logger::instance().error("Failed to initialize Win32 backend");
            return;
        }
        
        Logger::instance().info("Initializing DX11 backend...");
        if (!ImGui_ImplDX11_Init(d3dDevice_, d3dDeviceContext_)) {
            Logger::instance().error("Failed to initialize DX11 backend");
            return;
        }
        
        Logger::instance().info("Loading default font...");
        io.Fonts->AddFontDefault();
        
        Logger::instance().info("ImGui initialization complete");
        
    } catch (const std::exception& e) {
        Logger::instance().error("Exception in createImGuiContext: {}", e.what());
        throw;
    } catch (...) {
        Logger::instance().error("Unknown exception in createImGuiContext");
        throw;
    }
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
    
    // Claude Code style: dark gray background
    const float clear_color[4] = { 0.11f, 0.11f, 0.11f, 1.00f }; // #1C1C1C
    d3dDeviceContext_->OMSetRenderTargets(1, &mainRenderTargetView_, nullptr);
    d3dDeviceContext_->ClearRenderTargetView(mainRenderTargetView_, clear_color);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    swapChain_->Present(1, 0); // Present with vsync
}

void Application::renderUI() {
    // Claude Code style: no fancy background effects

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
            ImGui::MenuItem("IDE", nullptr, &showIDE_);
            ImGui::MenuItem("ImGui Demo", nullptr, &showDemoWindow_);
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("AI Providers")) {
            if (ImGui::MenuItem("Open ChatGPT")) {
                BrowserService::instance().openBrowser("chatgpt");
            }
            if (ImGui::MenuItem("Open DeepSeek")) {
                BrowserService::instance().openBrowser("deepseek");
            }
            if (ImGui::MenuItem("Open Claude")) {
                BrowserService::instance().openBrowser("claude");
            }
            if (ImGui::MenuItem("Open Kiro")) {
                BrowserService::instance().openBrowser("kiro");
            }
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

    // Quick access toolbar
    ImGui::SetNextWindowPos(ImVec2(200, 25));
    ImGui::SetNextWindowSize(ImVec2(600, 50));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.09f, 0.09f, 0.09f, 0.95f));
    ImGui::Begin("##Toolbar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    
    if (ImGui::Button("ChatGPT", ImVec2(100, 30))) {
        BrowserService::instance().openBrowser("chatgpt");
    }
    ImGui::SameLine();
    if (ImGui::Button("DeepSeek", ImVec2(100, 30))) {
        BrowserService::instance().openBrowser("deepseek");
    }
    ImGui::SameLine();
    if (ImGui::Button("Kiro", ImVec2(100, 30))) {
        BrowserService::instance().openBrowser("kiro");
    }
    ImGui::SameLine();
    if (ImGui::Button("IDE", ImVec2(100, 30))) {
        showIDE_ = true;
    }
    
    ImGui::End();
    ImGui::PopStyleColor();

    // Windows
    sidebarWindow_->render();
    chatWindow_->render();
    
    if (showLogWindow_) {
        logWindow_->render(&showLogWindow_);
    }
    
    if (showSettingsWindow_) {
        settingsWindow_->render(&showSettingsWindow_);
    }
    
    if (showIDE_) {
        ideWindow_->render(&showIDE_);
    }
    
    if (showDemoWindow_) {
        ImGui::ShowDemoWindow(&showDemoWindow_);
    }
    
    // Status bar
    statusBar_->render();
}

LRESULT WINAPI Application::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;
    
    Application* app = reinterpret_cast<Application*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
    
    switch (msg) {
        case WM_SIZE:
            if (app && app->d3dDevice_ != nullptr && wParam != SIZE_MINIMIZED) {
                // Правильный resize: не пересоздаем device, только RTV
                if (app->mainRenderTargetView_) {
                    app->mainRenderTargetView_->Release();
                    app->mainRenderTargetView_ = nullptr;
                }
                
                HRESULT hr = app->swapChain_->ResizeBuffers(0,
                    (UINT)LOWORD(lParam), (UINT)HIWORD(lParam),
                    DXGI_FORMAT_UNKNOWN, 0);
                
                if (SUCCEEDED(hr)) {
                    ID3D11Texture2D* pBackBuffer = nullptr;
                    app->swapChain_->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
                    if (pBackBuffer) {
                        app->d3dDevice_->CreateRenderTargetView(pBackBuffer, nullptr, &app->mainRenderTargetView_);
                        pBackBuffer->Release();
                    }
                }
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
