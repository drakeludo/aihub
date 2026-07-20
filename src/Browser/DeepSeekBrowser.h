#pragma once

#include "ChromeInstance.h"

class DeepSeekBrowser : public ChromeInstance {
    Q_OBJECT
    
public:
    explicit DeepSeekBrowser(QObject *parent = nullptr);
    
    // IBrowser interface
    QString sendPrompt(const QString& prompt) override;
    AIProvider provider() const override { return AIProvider::DeepSeek; }
    QString providerName() const override { return "DeepSeek"; }
    
protected:
    bool isLoggedIn() override;
    QString url() const override { return "https://chat.deepseek.com"; }
    
private:
    bool sendMessage(const QString& message);
    QString waitForResponse();
};
