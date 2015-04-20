#pragma once
#include <QString>
#include "Subspace.h"

enum AlgorithmId {
    DBSCAN,
    PREDECON,
    SUBCLU,
    QSCAN
};

enum DataStructure {
    BASIC,
    TI,
    PL,
    RTree
};

enum Measure {
    EUCLIDEAN,
    MANHATTAN
};

struct Settings {
    AlgorithmId algorithm;
    DataStructure dataStructure;
    Measure measure;
    double eps;
    int mi;
    double delta;
    int lambda;
    std::string path;
    bool odc;
    int n;
};

class MainWindow;

struct DrawSettings {
    int x;
    int y;
    double pointSize;
    bool draw;
    Subspace subspace;
};
