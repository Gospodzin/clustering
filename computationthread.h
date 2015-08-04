#pragma once

#include <QTCore>

#include "logging.h"
#include "Cluster.h"
#include "Subclu.h"
#include "Predecon.h"
#include "settings.h"
#include "DataLoader.h"
#include "DataWriter.h"
#include "Qscan.h"
#include "PLDataSet.h"
#include "SegTreeDataSet.h"
#include "VaFileDataSet.h"
#include "MTIDataSet.h"

class ComputationThread : public QThread
{
    Q_OBJECT
signals:
    void computed();
public:
    Settings sets;
    ComputationThread() {}
    std::shared_ptr<Data> data;
    std::map<Subspace, Clusters> result;
    double totalTime;

    void start(Settings sets, std::shared_ptr<Data> data) {
        this->sets = sets;
        this->data = data;
        QThread::start();
    }

private:
    void compute(Settings sets) {
        for(Point& p : *data) p.cid = NONE;

        switch(sets.algorithm) {
        case DBSCAN: runAlgorithm<Dbscan>(sets); break;
        case PREDECON: runAlgorithm<Predecon>(sets); break;
        case SUBCLU: runAlgorithm<Subclu>(sets); break;
        case QSCAN: runQscan(data.get(), sets); break;
        }
    }

    measures::MeasureId getMeasureId(Measure measure) {
        switch(measure) {
        case EUCLIDEAN:
            return measures::MeasureId::Euclidean;
        case MANHATTAN:
            return measures::MeasureId::Manhattan;
        default:
            throw -1;
        }
    }

    void runQscan(std::vector<Point>* data, Settings sets) {
        long start = clock();
        Qscan qscan(data, sets.eps, sets.mi);
        qscan.compute();
        result = qscan.getClusters();
        qscan.clean();
        this->totalTime = double(clock() - start) / CLOCKS_PER_SEC;
    }


    template<template<typename> class T>
    void runAlgorithm(Settings sets) {
        measures::MeasureId measureId = getMeasureId(sets.measure);
        switch(sets.dataStructure) {
        case BASIC: runAlgorithm<T, BasicDataSet>(data.get(), sets, {measureId}); break;
        case TI:    runAlgorithm<T, TIDataSet>(data.get(), sets, {measureId, referenceSelectors::max}); break;
        case MTI:    runAlgorithm<T, MTIDataSet>(data.get(), sets, {measureId, sets.n}); break;
        case PL:    runAlgorithm<T, PLDataSet>(data.get(), sets, {measureId, sets.n}); break;
        case SegTree: runAlgorithm<T, SegTreeDataSet>(data.get(), sets, {measureId, sets.eps, sets.n}); break;
        case VaFile: runAlgorithm<T, VaFileDataSet>(data.get(), sets, {measureId, sets.eps, sets.n}); break;
        }
    }

    template<template<typename> class T, class K>
    void runAlgorithm(Data* data, Settings sets, DataSet::Params params) {
        RunAlgorithmImpl<T, K>::call(this, data, sets, params);
    }

    template<template<typename> class T, class K>
    struct RunAlgorithmImpl {
      static void call(ComputationThread* thiz, Data* data, Settings sets, DataSet::Params params) {
          long start = clock();
          K dataSet(data, params);
          T<K> algorithm(&dataSet, {sets.eps, sets.mi, sets.delta, sets.lambda});
          algorithm.compute();
          thiz->result = algorithm.getClusters();
          algorithm.clean();
          thiz->totalTime = double(clock() - start) / CLOCKS_PER_SEC;
      }
    };

    template<class K>
    struct RunAlgorithmImpl<Subclu, K> {
      static void call(ComputationThread* thiz, Data* data, Settings sets, DataSet::Params params) {
          long start = clock();
          Subclu<K> subclu(data, {sets.eps, sets.mi, params, sets.odc});
          subclu.compute();
          thiz->result = subclu.getClusters();
          subclu.clean();
          thiz->totalTime = double(clock() - start) / CLOCKS_PER_SEC;
      }
    };

    void run()
    {
        compute(sets);
        emit computed();
    }
};
