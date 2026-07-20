#pragma once

#include <QObject>
#include <QString>

class SettingsManager : public QObject {
    Q_OBJECT
    
public:
    static SettingsManager& instance() {
        static SettingsManager inst;
        return inst;
    }
    
    void load() {
        // TODO: Load settings
    }
    
    void save() {
        // TODO: Save settings
    }
    
    QString theme() const { return "dark"; }
    
private:
    SettingsManager() = default;
};
