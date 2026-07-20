#pragma once

#include <QObject>

class WorkspaceManager : public QObject {
    Q_OBJECT
    
public:
    explicit WorkspaceManager(QObject *parent = nullptr) : QObject(parent) {}
    ~WorkspaceManager() override = default;
    
    void initialize() {
        // TODO: Implement workspace/project management
    }
};
