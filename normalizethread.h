#pragma once

#include <QTCore>
#include <chrono>
#include <memory>

#include "DataLoader.h"
#include "utils.h"

class NormalizeThread : public QThread
{
    Q_OBJECT
signals:
    void done();
public:
    std::shared_ptr<Data> data;
    Data* (*transformation)(std::vector<Point>&);

    double totalTime;

    void start(std::shared_ptr<Data> data, Data* (*transformation)(std::vector<Point>&)) {
        this->data = data;
        this->transformation = transformation;
        QThread::start();
    }

private:
    void run() {
        long start = clock();
        data = std::shared_ptr<Data>(transformation(*data));
        totalTime = double(clock() - start) / CLOCKS_PER_SEC;
        emit done();
    }
};
