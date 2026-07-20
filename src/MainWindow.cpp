#include "MainWindow.h"
#include "Sidebar/Sidebar.h"
#include "Chat/ChatWidget.h"
#include "Logger/LogWidget.h"
#include "Controllers/ChatController.h"
#include "Settings/SettingsDialog.h"
#include "Logger/Logger.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QStatusBar>
#include <QCloseEvent>
#include <QSettings>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , chatController_(std::make_unique<ChatController>())
{
    setWindowTitle("AI Hub");
    resize(1400, 900);
    setMinimumSize(1000, 600);
    
    setupUI();
    setupConnections();
    loadState();
    applyCustomStyles();
    
    // Initialize chat controller (handles all initialization)
    Logger::instance().info("Initializing chat controller...");
    chatController_->initialize();
    
    isInitialized_ = true;
    Logger::instance().info("Main window initialized");
}

MainWindow::~MainWindow() {
    saveState();
}

void MainWindow::setupUI() {
    // Central widget
    auto *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    auto *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Main splitter (Sidebar | Chat | Log)
    mainSplitter_ = new QSplitter(Qt::Horizontal, this);
    
    // Sidebar
    sidebar_ = new Sidebar(this);
    sidebar_->setMaximumWidth(300);
    sidebar_->setMinimumWidth(250);
    
    // Chat widget
    chatWidget_ = new ChatWidget(this);
    
    // Log widget (collapsible)
    logWidget_ = new LogWidget(this);
    logWidget_->setMaximumWidth(400);
    logWidget_->hide(); // Hidden by default
    
    mainSplitter_->addWidget(sidebar_);
    mainSplitter_->addWidget(chatWidget_);
    mainSplitter_->addWidget(logWidget_);
    
    // Set stretch factors
    mainSplitter_->setStretchFactor(0, 0); // Sidebar fixed
    mainSplitter_->setStretchFactor(1, 1); // Chat expandable
    mainSplitter_->setStretchFactor(2, 0); // Log fixed
    
    mainLayout->addWidget(mainSplitter_);
    
    // Status bar
    statusBar()->showMessage("Ready");
}

void MainWindow::setupConnections() {
    // Sidebar signals
    connect(sidebar_, &Sidebar::newChatRequested, this, &MainWindow::onNewChat);
    connect(sidebar_, &Sidebar::settingsRequested, this, &MainWindow::onSettings);
    connect(sidebar_, &Sidebar::chatSelected, this, &MainWindow::onChatSelected);
    connect(sidebar_, &Sidebar::themeChanged, this, &MainWindow::onThemeChanged);
    
    // New signals from enhanced sidebar
    connect(sidebar_, &Sidebar::providerChanged, this, [this](int provider) {
        chatController_->setProvider(static_cast<AIProvider>(provider));
    });
    
    connect(sidebar_, &Sidebar::strategyChanged, this, [this](int strategy) {
        chatController_->setProcessingStrategy(static_cast<ProcessingStrategy>(strategy));
    });
    
    // Chat widget signals
    connect(chatWidget_, &ChatWidget::messageSent, this, &MainWindow::onSendMessage);
    
    // ChatController signals
    connect(chatController_.get(), &ChatController::chatCreated, 
            this, [this](int chatId) {
        sidebar_->addChat(chatId, "New Chat");
        sidebar_->selectChat(chatId);
        chatWidget_->clearChat();
    });
    
    connect(chatController_.get(), &ChatController::messageReceived,
            this, &MainWindow::onMessageReceived);
    
    connect(chatController_.get(), &ChatController::messageError,
            this, &MainWindow::onMessageError);
    
    connect(chatController_.get(), &ChatController::processingStarted,
            this, &MainWindow::onProcessingStarted);
    
    connect(chatController_.get(), &ChatController::processingProgress,
            this, &MainWindow::onProcessingProgress);
    
    connect(chatController_.get(), &ChatController::processingCompleted,
            this, &MainWindow::onProcessingCompleted);
}

void MainWindow::loadState() {
    QSettings settings;
    
    // Window geometry
    if (settings.contains("geometry")) {
        restoreGeometry(settings.value("geometry").toByteArray());
    }
    
    // Splitter state
    if (settings.contains("splitter")) {
        mainSplitter_->restoreState(settings.value("splitter").toByteArray());
    }
}

void MainWindow::saveState() {
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    settings.setValue("splitter", mainSplitter_->saveState());
}

void MainWindow::applyCustomStyles() {
    // Dark theme custom styles
    setStyleSheet(R"(
        QMainWindow {
            background-color: #1e1e1e;
        }
        
        QSplitter::handle {
            background-color: #2d2d2d;
            width: 1px;
        }
        
        QSplitter::handle:hover {
            background-color: #ffb74d;
        }
        
        QStatusBar {
            background-color: #2b2b2b;
            color: #aaaaaa;
            border-top: 1px solid #3c3c3c;
        }
    )");
}

void MainWindow::onNewChat() {
    Logger::instance().info("Creating new chat...");
    chatController_->createNewChat();
    statusBar()->showMessage("New chat created", 3000);
}

void MainWindow::onSettings() {
    Logger::instance().info("Opening settings...");
    
    SettingsDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Logger::instance().info("Settings saved");
        statusBar()->showMessage("Settings updated", 3000);
    }
}

void MainWindow::onChatSelected(int chatId) {
    Logger::instance().info("Chat selected: " + std::to_string(chatId));
    
    chatController_->loadChat(chatId);
    
    // Load messages from database and display
    // TODO: Get messages from database via controller
    chatWidget_->clearChat();
    
    statusBar()->showMessage("Chat loaded", 2000);
}

void MainWindow::onSendMessage(const QString& message) {
    Logger::instance().info("Sending message: " + message.toStdString());
    
    // Add user message to UI
    chatWidget_->addMessage("You", message, MessageType::User);
    
    // Send via controller
    chatController_->sendMessage(message);
}

void MainWindow::onMessageReceived(int messageId, const QString& content) {
    Logger::instance().info("Message received: " + std::to_string(messageId));
    
    // Add AI response to UI
    chatWidget_->addMessage("AI", content, MessageType::Assistant);
}

void MainWindow::onMessageError(const QString& error) {
    Logger::instance().error("Message error: " + error.toStdString());
    
    chatWidget_->addMessage("System", "Error: " + error, MessageType::System);
    statusBar()->showMessage("Error: " + error, 5000);
}

void MainWindow::onProcessingStarted() {
    statusBar()->showMessage("Processing...");
    chatWidget_->setInputEnabled(false);
}

void MainWindow::onProcessingProgress(int progress, const QString& stage) {
    statusBar()->showMessage(QString("%1 (%2%)").arg(stage).arg(progress));
}

void MainWindow::onProcessingCompleted() {
    statusBar()->showMessage("Done", 3000);
    chatWidget_->setInputEnabled(true);
}

void MainWindow::onThemeChanged(const QString& theme) {
    Logger::instance().info("Theme changed: " + theme.toStdString());
    applyCustomStyles();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    Logger::instance().info("AI Hub shutting down...");
    
    // Ask for confirmation
    auto reply = QMessageBox::question(
        this,
        "Quit AI Hub",
        "Are you sure you want to quit?",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        saveState();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
}
