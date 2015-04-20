#pragma once

#include <QTCore>
#include <chrono>
#include <memory>

#include "DataLoader.h"
#include "settings.h"
#include "utils.h"
#include "referenceSelectors.h"

class LoadDataThread : public QThread
{
    Q_OBJECT
signals:
    void loaded();
public:
    std::string path;
    bool headers;

    std::shared_ptr<Data> data;

    double totalTime;

    void start(std::string path, bool headers) {
        this->path = path;
        this->headers = headers;
        QThread::start();
    }

private:
    void run() {
        long start = clock();
        data = std::shared_ptr<Data>(DataLoader(path).load(headers));
        this->totalTime = double(clock() - start) / CLOCKS_PER_SEC;
        emit loaded();
    }
};
