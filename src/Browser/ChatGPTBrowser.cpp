#include "ChatGPTBrowser.h"
#include "../Logger/Logger.h"

ChatGPTBrowser::ChatGPTBrowser(QObject *parent)
    : ChromeInstance("chatgpt", parent)
{
}

bool ChatGPTBrowser::isLoggedIn() {
    QString bodyText = executeJS("document.body.innerText");
    return !bodyText.contains("Log in") && 
           !bodyText.contains("Sign up") && 
           !bodyText.contains("Welcome to ChatGPT");
}

QString ChatGPTBrowser::sendPrompt(const QString& prompt) {
    Logger::instance().info("ChatGPT: Sending prompt...");
    
    if (!sendMessage(prompt)) {
        return "Error: Failed to send message";
    }
    
    QString response = waitForResponse();
    Logger::instance().info("ChatGPT: Response received (" + 
                          std::to_string(response.length()) + " chars)");
    
    return response;
}

bool ChatGPTBrowser::sendMessage(const QString& message) {
    // Count current messages
    int oldCount = getElementCount("article");
    
    // Escape message
    QString escapedMsg = message;
    escapedMsg.replace("\\", "\\\\");
    escapedMsg.replace("'", "\\'");
    escapedMsg.replace("\n", "\\n");
    
    QString js = QString(R"(
        (function() {
            let input = document.querySelector('textarea[name="prompt-textarea"]') || 
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
                let sendBtn = document.querySelector('button[data-testid="send-button"]');
                if (sendBtn && !sendBtn.disabled) {
                    sendBtn.click();
                } else {
                    const enterEvent = new KeyboardEvent('keydown', {
                        key: 'Enter',
                        code: 'Enter',
                        keyCode: 13,
                        bubbles: true
                    });
                    input.dispatchEvent(enterEvent);
                }
            }, 300);
            
            return {success: true};
        })()
    )").arg(escapedMsg);
    
    QString result = executeJS(js);
    return result.contains("true");
}

QString ChatGPTBrowser::waitForResponse() {
    // Wait for new message
    int oldCount = getElementCount("article");
    auto startTime = std::chrono::steady_clock::now();
    
    while (true) {
        int currentCount = getElementCount("article");
        if (currentCount > oldCount) {
            Logger::instance().info("ChatGPT: New message appeared");
            break;
        }
        
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - startTime
        ).count();
        
        if (elapsed >= 60) {
            Logger::instance().error("ChatGPT: Timeout waiting for new message");
            return "Error: Timeout";
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    // Wait for text to stabilize
    QString response = waitForTextChange(
        "[data-message-author-role='assistant'], article", 
        180000
    );
    
    return response;
}
