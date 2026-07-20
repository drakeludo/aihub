#include "DeepSeekBrowser.h"
#include "../Logger/Logger.h"

DeepSeekBrowser::DeepSeekBrowser(QObject *parent)
    : ChromeInstance("deepseek", parent)
{
}

bool DeepSeekBrowser::isLoggedIn() {
    QString bodyText = executeJS("document.body.innerText");
    return !bodyText.contains("Sign in") && !bodyText.contains("登录");
}

QString DeepSeekBrowser::sendPrompt(const QString& prompt) {
    Logger::instance().info("DeepSeek: Sending prompt...");
    
    if (!sendMessage(prompt)) {
        return "Error: Failed to send message";
    }
    
    QString response = waitForResponse();
    Logger::instance().info("DeepSeek: Response received (" + 
                          std::to_string(response.length()) + " chars)");
    
    return response;
}

bool DeepSeekBrowser::sendMessage(const QString& message) {
    // Escape message for JavaScript
    QString escapedMsg = message;
    escapedMsg.replace("\\", "\\\\");
    escapedMsg.replace("'", "\\'");
    escapedMsg.replace("\n", "\\n");
    
    QString js = QString(R"(
        (function() {
            let input = document.querySelector('textarea') || 
                       document.querySelector('[contenteditable="true"]');
            
            if (!input) return {success: false, error: 'Input not found'};
            
            input.focus();
            
            if (input.tagName === 'TEXTAREA') {
                const setter = Object.getOwnPropertyDescriptor(
                    window.HTMLTextAreaElement.prototype, 'value'
                ).set;
                setter.call(input, '%1');
                input.dispatchEvent(new Event('input', { bubbles: true }));
            } else {
                input.textContent = '%1';
                input.dispatchEvent(new Event('input', { bubbles: true }));
            }
            
            setTimeout(() => {
                const enterEvent = new KeyboardEvent('keydown', {
                    key: 'Enter',
                    code: 'Enter',
                    keyCode: 13,
                    bubbles: true
                });
                input.dispatchEvent(enterEvent);
            }, 200);
            
            return {success: true};
        })()
    )").arg(escapedMsg);
    
    QString result = executeJS(js);
    return result.contains("true");
}

QString DeepSeekBrowser::waitForResponse() {
    // Wait for message to appear
    if (!waitForElement("article, [class*='assistant']", 10000)) {
        Logger::instance().error("DeepSeek: Response element not found");
        return "Error: No response element";
    }
    
    // Wait for text to stabilize
    QString response = waitForTextChange("article, [class*='assistant']", 180000);
    return response;
}
