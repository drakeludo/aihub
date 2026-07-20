#pragma once

#include <QObject>

class PromptManager : public QObject {
    Q_OBJECT
    
public:
    explicit PromptManager(QObject *parent = nullptr) : QObject(parent) {}
    ~PromptManager() override = default;
    
    // TODO: Implement system prompts and templates
};
