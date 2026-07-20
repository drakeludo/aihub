#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QSpinBox>

class SettingsDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit SettingsDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Settings");
        resize(700, 500);
        setModal(true);
        
        auto* mainLayout = new QVBoxLayout(this);
        mainLayout->setSpacing(16);
        
        // Tab widget
        auto* tabs = new QTabWidget(this);
        
        // General tab
        auto* generalTab = createGeneralTab();
        tabs->addTab(generalTab, "General");
        
        // Browser tab
        auto* browserTab = createBrowserTab();
        tabs->addTab(browserTab, "Browser");
        
        // Appearance tab
        auto* appearanceTab = createAppearanceTab();
        tabs->addTab(appearanceTab, "Appearance");
        
        // Advanced tab
        auto* advancedTab = createAdvancedTab();
        tabs->addTab(advancedTab, "Advanced");
        
        mainLayout->addWidget(tabs);
        
        // Buttons
        auto* buttonLayout = new QHBoxLayout();
        buttonLayout->addStretch();
        
        auto* cancelBtn = new QPushButton("Cancel", this);
        auto* okBtn = new QPushButton("OK", this);
        okBtn->setDefault(true);
        
        buttonLayout->addWidget(cancelBtn);
        buttonLayout->addWidget(okBtn);
        
        mainLayout->addLayout(buttonLayout);
        
        // Styling
        setStyleSheet(R"(
            QDialog {
                background-color: #1e1e1e;
            }
            QLabel {
                color: #e0e0e0;
            }
            QPushButton {
                background-color: #2d2d2d;
                color: #e0e0e0;
                border: 1px solid #3c3c3c;
                border-radius: 6px;
                padding: 8px 24px;
                font-size: 10pt;
            }
            QPushButton:hover {
                background-color: #3d3d3d;
                border-color: #ffb74d;
            }
            QPushButton:default {
                background-color: #ffb74d;
                color: #1e1e1e;
                border: none;
            }
            QPushButton:default:hover {
                background-color: #ffc870;
            }
            QLineEdit, QComboBox, QSpinBox {
                background-color: #2d2d2d;
                color: #e0e0e0;
                border: 1px solid #3c3c3c;
                border-radius: 6px;
                padding: 6px;
            }
            QLineEdit:focus, QComboBox:focus, QSpinBox:focus {
                border-color: #ffb74d;
            }
            QTabWidget::pane {
                border: 1px solid #3c3c3c;
                border-radius: 6px;
                background-color: #252525;
            }
            QTabBar::tab {
                background-color: #2d2d2d;
                color: #e0e0e0;
                padding: 10px 20px;
                border: 1px solid #3c3c3c;
                border-bottom: none;
            }
            QTabBar::tab:selected {
                background-color: #252525;
                border-bottom: 2px solid #ffb74d;
            }
            QTabBar::tab:hover {
                background-color: #3d3d3d;
            }
            QGroupBox {
                color: #ffb74d;
                font-weight: bold;
                border: 1px solid #3c3c3c;
                border-radius: 6px;
                margin-top: 12px;
                padding-top: 12px;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                subcontrol-position: top left;
                padding: 0 8px;
            }
            QCheckBox {
                color: #e0e0e0;
            }
            QCheckBox::indicator {
                width: 18px;
                height: 18px;
                border: 2px solid #3c3c3c;
                border-radius: 4px;
                background-color: #2d2d2d;
            }
            QCheckBox::indicator:checked {
                background-color: #ffb74d;
                border-color: #ffb74d;
            }
        )");
        
        connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
        connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    }
    
private:
    QWidget* createGeneralTab() {
        auto* widget = new QWidget();
        auto* layout = new QVBoxLayout(widget);
        layout->setContentsMargins(16, 16, 16, 16);
        layout->setSpacing(16);
        
        // Default provider
        auto* providerGroup = new QGroupBox("Default AI Provider");
        auto* providerLayout = new QVBoxLayout(providerGroup);
        
        auto* providerCombo = new QComboBox();
        providerCombo->addItems({"Auto", "DeepSeek", "ChatGPT", "Kiro"});
        providerLayout->addWidget(providerCombo);
        
        // Default strategy
        auto* strategyGroup = new QGroupBox("Default Processing Strategy");
        auto* strategyLayout = new QVBoxLayout(strategyGroup);
        
        auto* strategyCombo = new QComboBox();
        strategyCombo->addItems({"Chain (DeepSeek → GPT)", "Single AI", "Parallel", "Best"});
        strategyLayout->addWidget(strategyCombo);
        
        // Auto-save
        auto* autoSaveCheck = new QCheckBox("Auto-save conversations");
        autoSaveCheck->setChecked(true);
        
        layout->addWidget(providerGroup);
        layout->addWidget(strategyGroup);
        layout->addWidget(autoSaveCheck);
        layout->addStretch();
        
        return widget;
    }
    
    QWidget* createBrowserTab() {
        auto* widget = new QWidget();
        auto* layout = new QVBoxLayout(widget);
        layout->setContentsMargins(16, 16, 16, 16);
        layout->setSpacing(16);
        
        // Profile paths
        auto* profileGroup = new QGroupBox("Browser Profiles");
        auto* profileLayout = new QVBoxLayout(profileGroup);
        
        auto* deepseekLabel = new QLabel("DeepSeek Profile:");
        auto* deepseekPath = new QLineEdit("%AppData%/AIHub/Profiles/deepseek");
        deepseekPath->setReadOnly(true);
        
        auto* gptLabel = new QLabel("ChatGPT Profile:");
        auto* gptPath = new QLineEdit("%AppData%/AIHub/Profiles/chatgpt");
        gptPath->setReadOnly(true);
        
        auto* kiroLabel = new QLabel("Kiro Profile:");
        auto* kiroPath = new QLineEdit("%AppData%/AIHub/Profiles/kiro");
        kiroPath->setReadOnly(true);
        
        profileLayout->addWidget(deepseekLabel);
        profileLayout->addWidget(deepseekPath);
        profileLayout->addWidget(gptLabel);
        profileLayout->addWidget(gptPath);
        profileLayout->addWidget(kiroLabel);
        profileLayout->addWidget(kiroPath);
        
        // Timeouts
        auto* timeoutGroup = new QGroupBox("Timeouts");
        auto* timeoutLayout = new QVBoxLayout(timeoutGroup);
        
        auto* responseLabel = new QLabel("Response timeout (seconds):");
        auto* responseTimeout = new QSpinBox();
        responseTimeout->setRange(30, 600);
        responseTimeout->setValue(180);
        
        auto* pageLoadLabel = new QLabel("Page load timeout (seconds):");
        auto* pageLoadTimeout = new QSpinBox();
        pageLoadTimeout->setRange(10, 120);
        pageLoadTimeout->setValue(30);
        
        timeoutLayout->addWidget(responseLabel);
        timeoutLayout->addWidget(responseTimeout);
        timeoutLayout->addWidget(pageLoadLabel);
        timeoutLayout->addWidget(pageLoadTimeout);
        
        layout->addWidget(profileGroup);
        layout->addWidget(timeoutGroup);
        layout->addStretch();
        
        return widget;
    }
    
    QWidget* createAppearanceTab() {
        auto* widget = new QWidget();
        auto* layout = new QVBoxLayout(widget);
        layout->setContentsMargins(16, 16, 16, 16);
        layout->setSpacing(16);
        
        // Theme
        auto* themeGroup = new QGroupBox("Theme");
        auto* themeLayout = new QVBoxLayout(themeGroup);
        
        auto* themeCombo = new QComboBox();
        themeCombo->addItems({
            "Dark (Default)",
            "Light",
            "Purple",
            "OLED",
            "Cyberpunk",
            "Dracula",
            "Nord",
            "Gruvbox"
        });
        themeLayout->addWidget(themeCombo);
        
        // Font size
        auto* fontGroup = new QGroupBox("Font");
        auto* fontLayout = new QVBoxLayout(fontGroup);
        
        auto* fontSizeLabel = new QLabel("Font size:");
        auto* fontSize = new QSpinBox();
        fontSize->setRange(8, 16);
        fontSize->setValue(10);
        fontSize->setSuffix(" pt");
        
        fontLayout->addWidget(fontSizeLabel);
        fontLayout->addWidget(fontSize);
        
        // UI options
        auto* showSidebarCheck = new QCheckBox("Show sidebar on startup");
        showSidebarCheck->setChecked(true);
        
        auto* showLogsCheck = new QCheckBox("Show logs panel");
        showLogsCheck->setChecked(false);
        
        layout->addWidget(themeGroup);
        layout->addWidget(fontGroup);
        layout->addWidget(showSidebarCheck);
        layout->addWidget(showLogsCheck);
        layout->addStretch();
        
        return widget;
    }
    
    QWidget* createAdvancedTab() {
        auto* widget = new QWidget();
        auto* layout = new QVBoxLayout(widget);
        layout->setContentsMargins(16, 16, 16, 16);
        layout->setSpacing(16);
        
        // Logging
        auto* logGroup = new QGroupBox("Logging");
        auto* logLayout = new QVBoxLayout(logGroup);
        
        auto* logLevelLabel = new QLabel("Log level:");
        auto* logLevel = new QComboBox();
        logLevel->addItems({"Debug", "Info", "Warning", "Error"});
        logLevel->setCurrentIndex(1);
        
        auto* logToFileCheck = new QCheckBox("Save logs to file");
        logToFileCheck->setChecked(true);
        
        logLayout->addWidget(logLevelLabel);
        logLayout->addWidget(logLevel);
        logLayout->addWidget(logToFileCheck);
        
        // Database
        auto* dbGroup = new QGroupBox("Database");
        auto* dbLayout = new QVBoxLayout(dbGroup);
        
        auto* dbPathLabel = new QLabel("Database location:");
        auto* dbPath = new QLineEdit("%AppData%/AIHub/database/aihub.db");
        dbPath->setReadOnly(true);
        
        auto* clearDbBtn = new QPushButton("Clear History");
        clearDbBtn->setMaximumWidth(150);
        
        dbLayout->addWidget(dbPathLabel);
        dbLayout->addWidget(dbPath);
        dbLayout->addWidget(clearDbBtn);
        
        // Performance
        auto* perfGroup = new QGroupBox("Performance");
        auto* perfLayout = new QVBoxLayout(perfGroup);
        
        auto* maxRetriesLabel = new QLabel("Max retries on error:");
        auto* maxRetries = new QSpinBox();
        maxRetries->setRange(0, 10);
        maxRetries->setValue(3);
        
        auto* threadPoolLabel = new QLabel("Thread pool size:");
        auto* threadPool = new QSpinBox();
        threadPool->setRange(1, 16);
        threadPool->setValue(4);
        
        perfLayout->addWidget(maxRetriesLabel);
        perfLayout->addWidget(maxRetries);
        perfLayout->addWidget(threadPoolLabel);
        perfLayout->addWidget(threadPool);
        
        layout->addWidget(logGroup);
        layout->addWidget(dbGroup);
        layout->addWidget(perfGroup);
        layout->addStretch();
        
        return widget;
    }
};
