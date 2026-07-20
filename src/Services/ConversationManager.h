#pragma once

#include <QObject>
#include <QString>

class ConversationManager : public QObject {
    Q_OBJECT
    
public:
    explicit ConversationManager(QObject *parent = nullptr) : QObject(parent) {}
    ~ConversationManager() override = default;
    
    void addMessage(int chatId, const QString& role, const QString& content) {
        // TODO: Implement conversation history management
        Q_UNUSED(chatId);
        Q_UNUSED(role);
        Q_UNUSED(content);
    }
};
