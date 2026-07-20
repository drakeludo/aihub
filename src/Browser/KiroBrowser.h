#pragma once

#include "ChromeInstance.h"

class KiroBrowser : public ChromeInstance {
    Q_OBJECT
    
public:
    explicit KiroBrowser(QObject *parent = nullptr);
    
    // IBrowser interface
    QString sendPrompt(const QString& prompt) override;
    AIProvider provider() const override { return AIProvider::Kiro; }
    QString providerName() const override { return "Kiro"; }
    
protected:
    bool isLoggedIn() override;
    QString url() const override { return "https://kiro.dev"; }
    
private:
    bool sendMessage(const QString& message);
    QString waitForResponse();
};
