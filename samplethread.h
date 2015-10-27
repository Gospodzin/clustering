#pragma once

#include <QTCore>
#include <chrono>
#include <memory>

#include "DataLoader.h"
#include "settings.h"
#include "utils.h"
#include "referenceSelectors.h"

class SampleThread : public QThread
{
    Q_OBJECT
signals:
    void done();
public:
    std::shared_ptr<Data> data;
    size_t size;
    size_t dims;
    int seed;

    double totalTime;

    void start(std::shared_ptr<Data> data, size_t size, size_t dims, int seed) {
        this->data = data;
        this->size = size;
        this->dims = dims;
        this->seed = seed;
        QThread::start();
    }

private:
    void run() {
        long start = clock();
        data = std::shared_ptr<Data>(utils::randomSample(*data, size, dims, seed));
        totalTime = double(clock() - start) / CLOCKS_PER_SEC;
        emit done();
    }
};
