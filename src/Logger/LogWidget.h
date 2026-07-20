#pragma once

#include <QWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QDateTime>

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

class LogWidget : public QWidget {
    Q_OBJECT
    
public:
    explicit LogWidget(QWidget *parent = nullptr) : QWidget(parent) {
        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(12, 12, 12, 12);
        layout->setSpacing(8);
        
        // Header
        auto* headerLayout = new QHBoxLayout();
        
        auto* titleLabel = new QLabel("Logs", this);
        QFont titleFont = titleLabel->font();
        titleFont.setPointSize(12);
        titleFont.setBold(true);
        titleLabel->setFont(titleFont);
        titleLabel->setStyleSheet("color: #ffb74d;");
        
        // Level filter
        levelFilter_ = new QComboBox(this);
        levelFilter_->addItem("All", -1);
        levelFilter_->addItem("Debug", 0);
        levelFilter_->addItem("Info", 1);
        levelFilter_->addItem("Warning", 2);
        levelFilter_->addItem("Error", 3);
        levelFilter_->setCurrentIndex(1); // Info by default
        levelFilter_->setFixedWidth(100);
        levelFilter_->setStyleSheet(R"(
            QComboBox {
                background-color: #2d2d2d;
                color: #e0e0e0;
                border: 1px solid #3c3c3c;
                border-radius: 4px;
                padding: 4px;
            }
        )");
        
        // Clear button
        clearBtn_ = new QPushButton("Clear", this);
        clearBtn_->setFixedSize(60, 24);
        clearBtn_->setStyleSheet(R"(
            QPushButton {
                background-color: #3d3d3d;
                color: #e0e0e0;
                border: none;
                border-radius: 4px;
                font-size: 9pt;
            }
            QPushButton:hover {
                background-color: #4d4d4d;
            }
        )");
        
        headerLayout->addWidget(titleLabel);
        headerLayout->addStretch();
        headerLayout->addWidget(levelFilter_);
        headerLayout->addWidget(clearBtn_);
        
        // Log text area
        logText_ = new QTextEdit(this);
        logText_->setReadOnly(true);
        logText_->setFont(QFont("Consolas", 9));
        logText_->setStyleSheet(R"(
            QTextEdit {
                background-color: #1a1a1a;
                color: #e0e0e0;
                border: 1px solid #3c3c3c;
                border-radius: 6px;
                padding: 8px;
            }
        )");
        
        layout->addLayout(headerLayout);
        layout->addWidget(logText_);
        
        setStyleSheet("background-color: #1e1e1e;");
        
        connect(clearBtn_, &QPushButton::clicked, this, &LogWidget::clear);
        connect(levelFilter_, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &LogWidget::onFilterChanged);
        
        // Add initial message
        addLog("Logs initialized", LogLevel::Info);
    }
    
    void addLog(const QString& message, LogLevel level = LogLevel::Info) {
        QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
        QString levelStr;
        QString color;
        
        switch (level) {
            case LogLevel::Debug:
                levelStr = "DEBUG";
                color = "#808080";
                break;
            case LogLevel::Info:
                levelStr = "INFO";
                color = "#4a9eff";
                break;
            case LogLevel::Warning:
                levelStr = "WARN";
                color = "#ffb74d";
                break;
            case LogLevel::Error:
                levelStr = "ERROR";
                color = "#ff6b6b";
                break;
        }
        
        // Check filter
        int currentFilter = levelFilter_->currentData().toInt();
        if (currentFilter >= 0 && static_cast<int>(level) < currentFilter) {
            return;
        }
        
        QString html = QString(
            "<span style='color: #808080;'>%1</span> "
            "<span style='color: %2; font-weight: bold;'>[%3]</span> "
            "<span style='color: #e0e0e0;'>%4</span>"
        ).arg(timestamp, color, levelStr, message);
        
        logText_->append(html);
        
        // Auto-scroll to bottom
        QTextCursor cursor = logText_->textCursor();
        cursor.movePosition(QTextCursor::End);
        logText_->setTextCursor(cursor);
    }
    
public slots:
    void clear() {
        logText_->clear();
        addLog("Logs cleared", LogLevel::Info);
    }
    
private slots:
    void onFilterChanged(int index) {
        Q_UNUSED(index);
        // Reload logs with new filter (would need to store logs in memory)
        // For now, just notify
        addLog("Filter changed", LogLevel::Info);
    }
    
private:
    QTextEdit* logText_;
    QComboBox* levelFilter_;
    QPushButton* clearBtn_;
};
