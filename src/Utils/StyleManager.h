#pragma once

#include <QObject>
#include <QString>

class StyleManager : public QObject {
    Q_OBJECT
    
public:
    static StyleManager& instance() {
        static StyleManager inst;
        return inst;
    }
    
    void applyTheme(const QString& theme) {
        Q_UNUSED(theme);
        // TODO: Apply theme
    }
    
private:
    StyleManager() = default;
};
