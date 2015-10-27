#pragma once
#include <QTCore>

#include "logging.h"

class GuiLogger : public QObject, public logging::Logger {
    Q_OBJECT

public:
    void log(std::string val, int depth) {
        emit logSignal(QString::fromStdString(val), depth);
    }

signals:
    void logSignal(QString msg, int depth);
};
