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
#include "RTreeDataSet.h"

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
        case DBSCAN:
            switch(sets.dataStructure) {
            case BASIC: runDbscan<BasicDataSet, int>(data.get(), sets, 0); break;
            case TI: runDbscan<TIDataSet, referenceSelectors::ReferenceSelector>(data.get(), sets, referenceSelectors::max); break;
            case PL: runDbscan<PLDataSet, int>(data.get(), sets, sets.n); break;
            case RTree: runDbscan<RTreeDataSet, RTreeDataSet::Params>(data.get(), sets, {sets.eps, sets.n}); break;
            } break;
        case PREDECON:
            switch(sets.dataStructure) {
            case BASIC: runPredecon<BasicDataSet>(data.get(), sets); break;
            case TI: runPredecon<TIDataSet>(data.get(), sets); break;
            } break;
        case SUBCLU:
            switch(sets.dataStructure) {
            case BASIC: runSubclu<BasicDataSet>(data.get(), sets); break;
            case TI: runSubclu<TIDataSet>(data.get(), sets); break;
            } break;
        case QSCAN:
            runQscan(data.get(), sets);
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

    template <typename T>
    void runSubclu(std::vector<Point>* data, Settings sets) {
        long start = clock();
        Subclu<T> subclu(data, sets.eps, sets.mi, sets.odc);
        subclu.compute();
        result = subclu.getClusters();
        subclu.clean();
        this->totalTime = double(clock() - start) / CLOCKS_PER_SEC;
    }

    template<typename T, typename K>
    void runDbscan(Data* data, Settings sets, K param) {
        long start = clock();
        T dataSet(data, getMeasureId(sets.measure), param);
        Dbscan<T> dbscan(&dataSet, sets.eps, sets.mi);
        dbscan.compute();
        result = dbscan.getClusters();
        dbscan.clean();
        this->totalTime = double(clock() - start) / CLOCKS_PER_SEC;
    }

    template<typename T>
    void runPredecon(std::vector<Point>* data, Settings sets) {
        long start = clock();
        T dataSet(data, getMeasureId(sets.measure), referenceSelectors::max);
        Predecon<T> predecon(&dataSet, sets.eps, sets.mi, sets.delta, sets.lambda);
        predecon.compute();
        result = predecon.getClusters();
        predecon.clean();
        this->totalTime = double(clock() - start) / CLOCKS_PER_SEC;
    }


    void run()
    {
        compute(sets);
        emit computed();
    }
};
