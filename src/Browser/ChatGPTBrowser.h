#pragma once

#include "ChromeInstance.h"

class ChatGPTBrowser : public ChromeInstance {
    Q_OBJECT
    
public:
    explicit ChatGPTBrowser(QObject *parent = nullptr);
    
    // IBrowser interface
    QString sendPrompt(const QString& prompt) override;
    AIProvider provider() const override { return AIProvider::ChatGPT; }
    QString providerName() const override { return "ChatGPT"; }
    
protected:
    bool isLoggedIn() override;
    QString url() const override { return "https://chatgpt.com/?model=gpt-4"; }
    
private:
    bool sendMessage(const QString& message);
    QString waitForResponse();
};
