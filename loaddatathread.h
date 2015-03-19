#pragma once

#include <QTCore>
#include <chrono>
#include <memory>

#include "DataLoader.h"
#include "settings.h"

class LoadDataThread : public QThread
{
    Q_OBJECT
signals:
    void loaded();
public:
    std::string path;
    std::shared_ptr<Data> data;
    double totalTime;

    void startWitPath(std::string path) {
        this->path = path;
        start();
    }

private:
    void run() {
        long start = clock();
        data = std::shared_ptr<Data>(DataLoader(path).load());
        this->totalTime = double(clock() - start) / CLOCKS_PER_SEC;
        emit loaded();
    }
};
