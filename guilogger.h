#pragma once
#include <QTCore>

#include "logging.h"

class GuiLogger : public QObject, public logging::Logger {
    Q_OBJECT

public:
    void log(std::string val) {
        emit logSignal(QString::fromStdString(val));
    }

signals:
    void logSignal(QString msg);
};
