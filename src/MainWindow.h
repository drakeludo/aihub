#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QSplitter>
#include <memory>

class Sidebar;
class ChatWidget;
class LogWidget;
class ChatController;

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    
protected:
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    
private slots:
    void onNewChat();
    void onSettings();
    void onChatSelected(int chatId);
    void onSendMessage(const QString& message);
    void onThemeChanged(const QString& theme);
    void onMessageReceived(int messageId, const QString& content);
    void onMessageError(const QString& error);
    void onProcessingStarted();
    void onProcessingProgress(int progress, const QString& stage);
    void onProcessingCompleted();
    
private:
    void setupUI();
    void setupConnections();
    void loadState();
    void saveState();
    void applyCustomStyles();
    
    // UI Components
    QSplitter *mainSplitter_;
    Sidebar *sidebar_;
    ChatWidget *chatWidget_;
    LogWidget *logWidget_;
    
    // Core components (only ChatController)
    std::unique_ptr<ChatController> chatController_;
    
    // State
    bool isInitialized_{false};
};
