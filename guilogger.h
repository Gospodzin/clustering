#pragma once
#include <QTextBrowser>
#include "logging.h"

struct GuiLogger : logging::Logger {
    QTextBrowser* logBrowser;
    GuiLogger(QTextBrowser* logBrowser) : logging::Logger(), logBrowser(logBrowser) {}

public:
    void log(std::string val) {
        logBrowser->append(QString::fromStdString(val));
    }
};
