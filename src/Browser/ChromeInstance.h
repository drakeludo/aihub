#pragma once

#include "../Core/IBrowser.h"
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QWebEnginePage>
#include <QString>
#include <memory>
#include <chrono>

class ChromeInstance : public IBrowser {
    Q_OBJECT
    
public:
    explicit ChromeInstance(const QString& profileName, QObject *parent = nullptr);
    virtual ~ChromeInstance();
    
    // IBrowser interface
    void initialize() override;
    bool isReady() const override { return ready_; }
    QString sendPrompt(const QString& prompt) override = 0;
    bool isLoggedIn() override = 0;
    AIProvider provider() const override = 0;
    QString providerName() const override = 0;
    void show() override;
    void hide() override;
    
    // Get web view
    QWebEngineView* webView() { return webView_.get(); }
    
protected:
    // Execute JavaScript
    QString executeJS(const QString& script);
    
    // Wait for element
    bool waitForElement(const QString& selector, int timeoutMs = 10000);
    
    // Wait for text change
    QString waitForTextChange(const QString& selector, int timeoutMs = 180000);
    
    // Get element text
    QString getElementText(const QString& selector);
    
    // Get element count
    int getElementCount(const QString& selector);
    
    // URL to navigate
    virtual QString url() const = 0;
    
    std::unique_ptr<QWebEngineView> webView_;
    std::unique_ptr<QWebEngineProfile> profile_;
    QString profilePath_;
    std::atomic<bool> ready_{false};
    
private:
    void setupWebView();
};
