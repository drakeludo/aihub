#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QFrame>
#include <QComboBox>
#include <QMouseEvent>
#include <QScrollArea>
#include <QMap>

class ChatListItem : public QWidget {
    Q_OBJECT
    
public:
    ChatListItem(int chatId, const QString& title, QWidget *parent = nullptr)
        : QWidget(parent), chatId_(chatId)
    {
        auto* layout = new QHBoxLayout(this);
        layout->setContentsMargins(12, 8, 12, 8);
        
        titleLabel_ = new QLabel(title, this);
        titleLabel_->setStyleSheet("color: #e0e0e0; font-size: 10pt;");
        
        layout->addWidget(titleLabel_);
        layout->addStretch();
        
        setStyleSheet(R"(
            ChatListItem {
                background-color: transparent;
                border-radius: 6px;
            }
            ChatListItem:hover {
                background-color: #2d2d2d;
            }
        )");
        
        setCursor(Qt::PointingHandCursor);
    }
    
    int chatId() const { return chatId_; }
    
    void setSelected(bool selected) {
        if (selected) {
            setStyleSheet(R"(
                ChatListItem {
                    background-color: #ffb74d;
                    border-radius: 6px;
                }
                QLabel {
                    color: #1e1e1e;
                    font-weight: bold;
                }
            )");
        } else {
            setStyleSheet(R"(
                ChatListItem {
                    background-color: transparent;
                    border-radius: 6px;
                }
                ChatListItem:hover {
                    background-color: #2d2d2d;
                }
                QLabel {
                    color: #e0e0e0;
                }
            )");
        }
    }
    
signals:
    void clicked(int chatId);
    
protected:
    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            emit clicked(chatId_);
        }
        QWidget::mousePressEvent(event);
    }
    
private:
    int chatId_;
    QLabel* titleLabel_;
};

class Sidebar : public QWidget {
    Q_OBJECT
    
public:
    explicit Sidebar(QWidget *parent = nullptr) : QWidget(parent) {
        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(12, 12, 12, 12);
        layout->setSpacing(12);
        
        // Header
        auto* headerLabel = new QLabel("AI Hub", this);
        QFont headerFont = headerLabel->font();
        headerFont.setPointSize(16);
        headerFont.setBold(true);
        headerLabel->setFont(headerFont);
        headerLabel->setStyleSheet("color: #ffb74d;");
        
        // New chat button
        newChatBtn_ = new QPushButton("+ New Chat", this);
        newChatBtn_->setFixedHeight(40);
        newChatBtn_->setCursor(Qt::PointingHandCursor);
        newChatBtn_->setStyleSheet(R"(
            QPushButton {
                background-color: #ffb74d;
                color: #1e1e1e;
                border: none;
                border-radius: 8px;
                font-weight: bold;
                font-size: 11pt;
            }
            QPushButton:hover {
                background-color: #ffc870;
            }
            QPushButton:pressed {
                background-color: #e69a3d;
            }
        )");
        
        // Provider selector
        auto* providerLabel = new QLabel("AI Provider:", this);
        providerLabel->setStyleSheet("color: #aaaaaa; font-size: 9pt;");
        
        providerCombo_ = new QComboBox(this);
        providerCombo_->addItem("Auto", -1);
        providerCombo_->addItem("DeepSeek", 0);
        providerCombo_->addItem("ChatGPT", 1);
        providerCombo_->addItem("Kiro", 2);
        providerCombo_->setStyleSheet(R"(
            QComboBox {
                background-color: #2d2d2d;
                color: #e0e0e0;
                border: 1px solid #3c3c3c;
                border-radius: 6px;
                padding: 6px;
            }
            QComboBox:hover {
                border-color: #ffb74d;
            }
            QComboBox::drop-down {
                border: none;
            }
            QComboBox QAbstractItemView {
                background-color: #2d2d2d;
                color: #e0e0e0;
                selection-background-color: #ffb74d;
                selection-color: #1e1e1e;
            }
        )");
        
        // Strategy selector
        auto* strategyLabel = new QLabel("Strategy:", this);
        strategyLabel->setStyleSheet("color: #aaaaaa; font-size: 9pt;");
        
        strategyCombo_ = new QComboBox(this);
        strategyCombo_->addItem("Chain (DS→GPT)", 1);
        strategyCombo_->addItem("Single AI", 0);
        strategyCombo_->addItem("Parallel", 2);
        strategyCombo_->addItem("Best", 3);
        strategyCombo_->setStyleSheet(providerCombo_->styleSheet());
        
        // Chat list
        auto* chatsLabel = new QLabel("Chats:", this);
        chatsLabel->setStyleSheet("color: #aaaaaa; font-size: 9pt;");
        
        chatListWidget_ = new QWidget(this);
        chatListLayout_ = new QVBoxLayout(chatListWidget_);
        chatListLayout_->setContentsMargins(0, 0, 0, 0);
        chatListLayout_->setSpacing(4);
        chatListLayout_->addStretch();
        
        auto* scrollArea = new QScrollArea(this);
        scrollArea->setWidget(chatListWidget_);
        scrollArea->setWidgetResizable(true);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setStyleSheet(R"(
            QScrollArea {
                border: none;
                background-color: transparent;
            }
            QScrollBar:vertical {
                background-color: #2d2d2d;
                width: 8px;
                border-radius: 4px;
            }
            QScrollBar::handle:vertical {
                background-color: #4d4d4d;
                border-radius: 4px;
            }
        )");
        
        // Separator
        auto* separator = new QFrame(this);
        separator->setFrameShape(QFrame::HLine);
        separator->setStyleSheet("background-color: #3c3c3c;");
        
        // Settings button
        settingsBtn_ = new QPushButton("⚙ Settings", this);
        settingsBtn_->setFixedHeight(36);
        settingsBtn_->setCursor(Qt::PointingHandCursor);
        settingsBtn_->setStyleSheet(R"(
            QPushButton {
                background-color: #2d2d2d;
                color: #e0e0e0;
                border: 1px solid #3c3c3c;
                border-radius: 6px;
                font-size: 10pt;
                text-align: left;
                padding-left: 12px;
            }
            QPushButton:hover {
                background-color: #3d3d3d;
                border-color: #ffb74d;
            }
        )");
        
        layout->addWidget(headerLabel);
        layout->addWidget(newChatBtn_);
        layout->addSpacing(8);
        layout->addWidget(providerLabel);
        layout->addWidget(providerCombo_);
        layout->addWidget(strategyLabel);
        layout->addWidget(strategyCombo_);
        layout->addSpacing(8);
        layout->addWidget(chatsLabel);
        layout->addWidget(scrollArea, 1);
        layout->addWidget(separator);
        layout->addWidget(settingsBtn_);
        
        setStyleSheet("background-color: #252525;");
        
        connect(newChatBtn_, &QPushButton::clicked, this, &Sidebar::newChatRequested);
        connect(settingsBtn_, &QPushButton::clicked, this, &Sidebar::settingsRequested);
        connect(providerCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, [this](int index) {
            Q_UNUSED(index);
            emit providerChanged(providerCombo_->currentData().toInt());
        });
        connect(strategyCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, [this](int index) {
            Q_UNUSED(index);
            emit strategyChanged(strategyCombo_->currentData().toInt());
        });
    }
    
    void addChat(int chatId, const QString& title) {
        // Remove stretch
        if (chatListLayout_->count() > 0) {
            auto* item = chatListLayout_->itemAt(chatListLayout_->count() - 1);
            if (item->spacerItem()) {
                chatListLayout_->removeItem(item);
                delete item;
            }
        }
        
        auto* chatItem = new ChatListItem(chatId, title, chatListWidget_);
        connect(chatItem, &ChatListItem::clicked, this, [this](int id) {
            selectChat(id);
            emit chatSelected(id);
        });
        
        chatListLayout_->insertWidget(0, chatItem); // Insert at top
        chatListLayout_->addStretch();
        
        chatItems_[chatId] = chatItem;
    }
    
    void selectChat(int chatId) {
        for (auto* item : chatItems_) {
            item->setSelected(false);
        }
        
        if (chatItems_.contains(chatId)) {
            chatItems_[chatId]->setSelected(true);
            currentChatId_ = chatId;
        }
    }
    
signals:
    void newChatRequested();
    void settingsRequested();
    void chatSelected(int chatId);
    void themeChanged(const QString& theme);
    void providerChanged(int provider);
    void strategyChanged(int strategy);
    
private:
    QPushButton* newChatBtn_;
    QPushButton* settingsBtn_;
    QComboBox* providerCombo_;
    QComboBox* strategyCombo_;
    QWidget* chatListWidget_;
    QVBoxLayout* chatListLayout_;
    QMap<int, ChatListItem*> chatItems_;
    int currentChatId_{-1};
};
