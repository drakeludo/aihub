/*
 * AI Hub - Unified AI Chat Desktop Application
 * Qt6 + C++23 Native Application
 */

#include <QApplication>
#include <QStyleFactory>
#include <QFont>
#include "MainWindow.h"
#include "Logger/Logger.h"
#include "Settings/SettingsManager.h"
#include "Utils/StyleManager.h"
#include "Core/BrowserFactory.h"
#include "Browser/DeepSeekBrowser.h"
#include "Browser/ChatGPTBrowser.h"
#include "Browser/KiroBrowser.h"

void registerBrowsers() {
    auto& factory = BrowserFactory::instance();
    
    // Register DeepSeek
    factory.registerBrowser(AIProvider::DeepSeek, []() {
        return std::make_unique<DeepSeekBrowser>();
    });
    
    // Register ChatGPT
    factory.registerBrowser(AIProvider::ChatGPT, []() {
        return std::make_unique<ChatGPTBrowser>();
    });
    
    // Register Kiro
    factory.registerBrowser(AIProvider::Kiro, []() {
        return std::make_unique<KiroBrowser>();
    });
    
    Logger::instance().info("Browsers registered: " + 
                          std::to_string(factory.supportedProviders().size()));
}

int main(int argc, char *argv[]) {
    // Enable High DPI scaling
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    
    QApplication app(argc, argv);
    
    // Application metadata
    QApplication::setApplicationName("AI Hub");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("AIHub");
    QApplication::setOrganizationDomain("aihub.local");
    
    // Initialize logger
    Logger::instance().initialize();
    Logger::instance().info("AI Hub starting...");
    
    // Register browsers
    registerBrowsers();
    
    // Load settings
    auto& settings = SettingsManager::instance();
    settings.load();
    
    // Apply theme
    StyleManager::instance().applyTheme(settings.theme());
    
    // Set font
    QFont font("Segoe UI", 10);
    QApplication::setFont(font);
    
    // Create main window
    MainWindow window;
    window.show();
    
    Logger::instance().info("AI Hub started successfully");
    
    return QApplication::exec();
}
