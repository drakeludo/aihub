#pragma once

#include <QString>
#include <QObject>
#include "AIRequest.h"

class IBrowser : public QObject {
    Q_OBJECT
    
public:
    virtual ~IBrowser() = default;
    
    // Initialize browser
    virtual void initialize() = 0;
    
    // Check if ready
    virtual bool isReady() const = 0;
    
    // Send prompt and get response
    virtual QString sendPrompt(const QString& prompt) = 0;
    
    // Check if logged in
    virtual bool isLoggedIn() = 0;
    
    // Get provider info
    virtual AIProvider provider() const = 0;
    virtual QString providerName() const = 0;
    
    // Show/hide browser window
    virtual void show() = 0;
    virtual void hide() = 0;
    
signals:
    void ready();
    void error(const QString& message);
    void responseReceived(const QString& response);
    void loginRequired();
};
