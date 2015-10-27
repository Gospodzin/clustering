#pragma once

#include <QTCore>
#include <chrono>
#include <memory>

#include "DataLoader.h"
#include "settings.h"
#include "utils.h"
#include "referenceSelectors.h"

class DataStatsThread : public QThread
{
    Q_OBJECT
signals:
    void done();
public:
    std::shared_ptr<Data> data;

    std::vector<double> mins;
    std::vector<double> maxs;
    std::vector<double> means;
    std::vector<double> medians;
    std::vector<double> meanDeviations;
    std::vector<double> standardDeviations;

    double totalTime;

    void start(std::shared_ptr<Data> data) {
        this->data = data;
        QThread::start();
    }

private:
    void run() {
        long start = clock();
        means = utils::calcMeans(*data);
        medians = utils::calcMedians(*data);
        meanDeviations = utils::calcMeanDeviations(*data);
        standardDeviations = utils::calcStandardDeviations(*data);
        mins = referenceSelectors::min(*data);
        maxs = referenceSelectors::max(*data);
        this->totalTime = double(clock() - start) / CLOCKS_PER_SEC;
        emit done();
    }
};
