#pragma once

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QVector>

struct Message {
    int id;
    QString role;
    QString content;
    QString timestamp;
};

class DatabaseManager : public QObject {
    Q_OBJECT
    
public:
    explicit DatabaseManager(QObject *parent = nullptr) : QObject(parent) {}
    ~DatabaseManager() override = default;
    
    void initialize() {
        // TODO: Initialize SQLite database
    }
    
    int createChat(const QString& title) {
        Q_UNUSED(title);
        return ++lastChatId_;
    }
    
    void deleteChat(int chatId) {
        Q_UNUSED(chatId);
    }
    
    void clearMessages(int chatId) {
        Q_UNUSED(chatId);
    }
    
    int saveMessage(int chatId, const QString& role, const QString& content) {
        Q_UNUSED(chatId);
        Q_UNUSED(role);
        Q_UNUSED(content);
        return ++lastMessageId_;
    }
    
    void updateMessage(int messageId, const QString& content) {
        Q_UNUSED(messageId);
        Q_UNUSED(content);
    }
    
    void deleteMessage(int messageId) {
        Q_UNUSED(messageId);
    }
    
    QString getMessage(int messageId) {
        Q_UNUSED(messageId);
        return "";
    }
    
    QVector<Message> loadMessages(int chatId) {
        Q_UNUSED(chatId);
        return {};
    }
    
private:
    int lastChatId_{0};
    int lastMessageId_{0};
};
