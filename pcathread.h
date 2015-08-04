#pragma once

#include <QTCore>
#include <chrono>
#include <memory>

#include "DataLoader.h"
#include "settings.h"
#include "utils.h"
#include "referenceSelectors.h"

class PcaThread : public QThread
{
    Q_OBJECT
signals:
    void done();
public:
    std::shared_ptr<Data> data;
    size_t dims;

    double totalTime;

    void start(std::shared_ptr<Data> data, size_t dims) {
        this->data = data;
        this->dims = dims;
        QThread::start();
    }

private:
    void run() {
        long start = clock();
        data = std::shared_ptr<Data>(utils::pca(*data, dims));
        emit done();
    }
};
