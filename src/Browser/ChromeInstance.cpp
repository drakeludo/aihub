#include "ChromeInstance.h"
#include "../Logger/Logger.h"
#include <QEventLoop>
#include <QTimer>
#include <QStandardPaths>
#include <QDir>
#include <thread>

ChromeInstance::ChromeInstance(const QString& profileName, QObject *parent)
    : IBrowser(parent)
{
    // Create profile directory
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    profilePath_ = dataPath + "/Profiles/" + profileName;
    
    QDir dir;
    dir.mkpath(profilePath_);
    
    Logger::instance().info("Chrome instance created: " + profileName.toStdString());
}

ChromeInstance::~ChromeInstance() {
    if (webView_) {
        webView_->close();
    }
}

void ChromeInstance::initialize() {
    Logger::instance().info("Initializing Chrome instance...");
    
    // Create profile
    profile_ = std::make_unique<QWebEngineProfile>(profilePath_);
    profile_->setPersistentCookiesPolicy(QWebEngineProfile::ForcePersistentCookies);
    
    // Create web view
    webView_ = std::make_unique<QWebEngineView>();
    webView_->setPage(new QWebEnginePage(profile_.get(), webView_.get()));
    webView_->resize(1280, 900);
    webView_->setWindowTitle("AI Hub Browser");
    
    // Load URL
    Logger::instance().info("Loading URL: " + url().toStdString());
    webView_->load(QUrl(url()));
    
    // Wait for page load
    QEventLoop loop;
    connect(webView_.get(), &QWebEngineView::loadFinished, &loop, &QEventLoop::quit);
    QTimer::singleShot(30000, &loop, &QEventLoop::quit); // Timeout
    loop.exec();
    
    // Wait for Cloudflare
    std::this_thread::sleep_for(std::chrono::seconds(5));
    
    // Check if logged in
    if (!isLoggedIn()) {
        Logger::instance().warning("Not logged in - please log in manually");
        show(); // Show browser for user to login
    }
    
    ready_ = true;
    emit ready();
    
    Logger::instance().info("Chrome instance ready");
}

QString ChromeInstance::executeJS(const QString& script) {
    QString result;
    QEventLoop loop;
    
    webView_->page()->runJavaScript(script, [&result, &loop](const QVariant& v) {
        result = v.toString();
        loop.quit();
    });
    
    QTimer::singleShot(5000, &loop, &QEventLoop::quit); // Timeout
    loop.exec();
    
    return result;
}

bool ChromeInstance::waitForElement(const QString& selector, int timeoutMs) {
    auto startTime = std::chrono::steady_clock::now();
    
    while (true) {
        QString js = QString(
            "(function() {"
            "  let el = document.querySelector('%1');"
            "  return el !== null;"
            "})()"
        ).arg(selector);
        
        QString result = executeJS(js);
        if (result == "true") {
            return true;
        }
        
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - startTime
        ).count();
        
        if (elapsed >= timeoutMs) {
            return false;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

QString ChromeInstance::waitForTextChange(const QString& selector, int timeoutMs) {
    QString lastText = getElementText(selector);
    int stableCount = 0;
    auto startTime = std::chrono::steady_clock::now();
    
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        QString currentText = getElementText(selector);
        
        if (currentText == lastText && !currentText.isEmpty()) {
            stableCount++;
            if (stableCount >= 3) { // 6 seconds stable
                Logger::instance().info("Text stabilized: " + 
                                      std::to_string(currentText.length()) + " chars");
                return currentText;
            }
        } else {
            stableCount = 0;
            lastText = currentText;
        }
        
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - startTime
        ).count();
        
        if (elapsed >= timeoutMs) {
            Logger::instance().warning("Timeout waiting for text change");
            return lastText;
        }
    }
}

QString ChromeInstance::getElementText(const QString& selector) {
    QString js = QString(
        "(function() {"
        "  let elements = document.querySelectorAll('%1');"
        "  if (elements.length > 0) {"
        "    return elements[elements.length - 1].innerText;"
        "  }"
        "  return '';"
        "})()"
    ).arg(selector);
    
    return executeJS(js);
}

int ChromeInstance::getElementCount(const QString& selector) {
    QString js = QString(
        "(function() {"
        "  return document.querySelectorAll('%1').length;"
        "})()"
    ).arg(selector);
    
    return executeJS(js).toInt();
}

void ChromeInstance::show() {
    if (webView_) {
        webView_->show();
        webView_->raise();
        webView_->activateWindow();
    }
}

void ChromeInstance::hide() {
    if (webView_) {
        webView_->hide();
    }
}
