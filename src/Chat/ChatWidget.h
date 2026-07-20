#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QKeyEvent>
#include <QTimer>

enum class MessageType {
    User,
    Assistant,
    System
};

class MessageBubble : public QFrame {
    Q_OBJECT
    
public:
    MessageBubble(const QString& sender, const QString& content, MessageType type, QWidget *parent = nullptr)
        : QFrame(parent), type_(type)
    {
        setFrameStyle(QFrame::NoFrame);
        
        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(16, 12, 16, 12);
        layout->setSpacing(8);
        
        // Sender label
        auto* senderLabel = new QLabel(sender, this);
        QFont senderFont = senderLabel->font();
        senderFont.setBold(true);
        senderFont.setPointSize(10);
        senderLabel->setFont(senderFont);
        
        // Content
        auto* contentLabel = new QLabel(content, this);
        contentLabel->setWordWrap(true);
        contentLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
        contentLabel->setFont(QFont("Segoe UI", 10));
        
        layout->addWidget(senderLabel);
        layout->addWidget(contentLabel);
        
        // Styling based on type
        QString bgColor, textColor, senderColor;
        if (type == MessageType::User) {
            bgColor = "#2b5278";
            textColor = "#ffffff";
            senderColor = "#a8d4ff";
        } else if (type == MessageType::Assistant) {
            bgColor = "#2d2d2d";
            textColor = "#e0e0e0";
            senderColor = "#ffb74d";
        } else {
            bgColor = "#3d2d2d";
            textColor = "#cccccc";
            senderColor = "#ff6b6b";
        }
        
        setStyleSheet(QString(R"(
            MessageBubble {
                background-color: %1;
                border-radius: 12px;
                margin: 4px;
            }
        )").arg(bgColor));
        
        senderLabel->setStyleSheet(QString("color: %1;").arg(senderColor));
        contentLabel->setStyleSheet(QString("color: %1;").arg(textColor));
        
        setMaximumWidth(800);
    }
    
private:
    MessageType type_;
};

class InputWidget : public QWidget {
    Q_OBJECT
    
public:
    explicit InputWidget(QWidget *parent = nullptr) : QWidget(parent) {
        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(16, 8, 16, 16);
        layout->setSpacing(8);
        
        // Input area
        inputEdit_ = new QTextEdit(this);
        inputEdit_->setPlaceholderText("Type your message... (Ctrl+Enter to send)");
        inputEdit_->setMaximumHeight(120);
        inputEdit_->setFont(QFont("Segoe UI", 10));
        
        // Send button
        sendBtn_ = new QPushButton("Send", this);
        sendBtn_->setFixedSize(100, 36);
        sendBtn_->setCursor(Qt::PointingHandCursor);
        
        auto* btnLayout = new QHBoxLayout();
        btnLayout->addStretch();
        btnLayout->addWidget(sendBtn_);
        
        layout->addWidget(inputEdit_);
        layout->addLayout(btnLayout);
        
        // Styling
        inputEdit_->setStyleSheet(R"(
            QTextEdit {
                background-color: #2d2d2d;
                color: #e0e0e0;
                border: 2px solid #3c3c3c;
                border-radius: 8px;
                padding: 12px;
                font-size: 10pt;
            }
            QTextEdit:focus {
                border-color: #ffb74d;
            }
        )");
        
        sendBtn_->setStyleSheet(R"(
            QPushButton {
                background-color: #ffb74d;
                color: #1e1e1e;
                border: none;
                border-radius: 8px;
                font-weight: bold;
                font-size: 10pt;
            }
            QPushButton:hover {
                background-color: #ffc870;
            }
            QPushButton:pressed {
                background-color: #e69a3d;
            }
            QPushButton:disabled {
                background-color: #4d4d4d;
                color: #808080;
            }
        )");
        
        connect(sendBtn_, &QPushButton::clicked, this, &InputWidget::onSendClicked);
        connect(inputEdit_, &QTextEdit::textChanged, this, [this]() {
            sendBtn_->setEnabled(!inputEdit_->toPlainText().trimmed().isEmpty());
        });
        
        // Install event filter for Ctrl+Enter
        inputEdit_->installEventFilter(this);
        
        sendBtn_->setEnabled(false);
    }
    
    void clear() {
        inputEdit_->clear();
    }
    
    void setEnabled(bool enabled) {
        inputEdit_->setEnabled(enabled);
        sendBtn_->setEnabled(enabled && !inputEdit_->toPlainText().trimmed().isEmpty());
    }
    
signals:
    void messageSent(const QString& message);
    
private slots:
    void onSendClicked() {
        QString text = inputEdit_->toPlainText().trimmed();
        if (!text.isEmpty()) {
            emit messageSent(text);
            clear();
        }
    }
    
protected:
    bool eventFilter(QObject* obj, QEvent* event) override {
        if (obj == inputEdit_ && event->type() == QEvent::KeyPress) {
            auto* keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Return && keyEvent->modifiers() & Qt::ControlModifier) {
                onSendClicked();
                return true;
            }
        }
        return QWidget::eventFilter(obj, event);
    }
    
private:
    QTextEdit* inputEdit_;
    QPushButton* sendBtn_;
};

class ChatWidget : public QWidget {
    Q_OBJECT
    
public:
    explicit ChatWidget(QWidget *parent = nullptr) : QWidget(parent) {
        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        
        // Scroll area for messages
        scrollArea_ = new QScrollArea(this);
        scrollArea_->setWidgetResizable(true);
        scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        
        // Messages container
        messagesWidget_ = new QWidget();
        messagesLayout_ = new QVBoxLayout(messagesWidget_);
        messagesLayout_->setContentsMargins(16, 16, 16, 16);
        messagesLayout_->setSpacing(12);
        messagesLayout_->addStretch();
        
        scrollArea_->setWidget(messagesWidget_);
        
        // Input widget
        inputWidget_ = new InputWidget(this);
        
        layout->addWidget(scrollArea_);
        layout->addWidget(inputWidget_);
        
        // Styling
        setStyleSheet(R"(
            ChatWidget {
                background-color: #1e1e1e;
            }
            QScrollArea {
                border: none;
                background-color: #1e1e1e;
            }
            QScrollBar:vertical {
                background-color: #2d2d2d;
                width: 12px;
                border-radius: 6px;
            }
            QScrollBar::handle:vertical {
                background-color: #4d4d4d;
                border-radius: 6px;
                min-height: 20px;
            }
            QScrollBar::handle:vertical:hover {
                background-color: #5d5d5d;
            }
        )");
        
        connect(inputWidget_, &InputWidget::messageSent, this, &ChatWidget::messageSent);
    }
    
    void addMessage(const QString& sender, const QString& message, MessageType type) {
        // Remove stretch before adding message
        if (messagesLayout_->count() > 0) {
            auto* item = messagesLayout_->itemAt(messagesLayout_->count() - 1);
            if (item->spacerItem()) {
                messagesLayout_->removeItem(item);
                delete item;
            }
        }
        
        auto* bubble = new MessageBubble(sender, message, type, messagesWidget_);
        messagesLayout_->addWidget(bubble);
        messagesLayout_->addStretch();
        
        // Scroll to bottom
        QTimer::singleShot(100, this, [this]() {
            scrollArea_->verticalScrollBar()->setValue(
                scrollArea_->verticalScrollBar()->maximum()
            );
        });
    }
    
    void clearChat() {
        // Remove all messages
        while (messagesLayout_->count() > 0) {
            auto* item = messagesLayout_->takeAt(0);
            if (item->widget()) {
                item->widget()->deleteLater();
            }
            delete item;
        }
        messagesLayout_->addStretch();
        inputWidget_->clear();
    }
    
    void loadMessages(const auto& messages) {
        clearChat();
        for (const auto& msg : messages) {
            MessageType type = MessageType::Assistant;
            if (msg.role == "user") {
                type = MessageType::User;
            } else if (msg.role == "system") {
                type = MessageType::System;
            }
            addMessage(msg.role, msg.content, type);
        }
    }
    
    void setInputEnabled(bool enabled) {
        inputWidget_->setEnabled(enabled);
    }
    
signals:
    void messageSent(const QString& message);
    
private:
    QScrollArea* scrollArea_;
    QWidget* messagesWidget_;
    QVBoxLayout* messagesLayout_;
    InputWidget* inputWidget_;
};
