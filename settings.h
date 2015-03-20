#pragma once
#include <QString>
#include "Subspace.h"

enum Algorithm {
    DBSCAN,
    PREDECON,
    SUBCLU
};

enum DataStructure {
    BASIC,
    TI
};

enum Measure {
    EUCLIDEAN,
    MANHATTAN
};

struct Settings {
    Algorithm algorithm;
    DataStructure dataStructure;
    Measure measure;
    double eps;
    int mi;
    double delta;
    int lambda;
    std::string path;
    bool writeOut;
    bool odc;
};

class MainWindow;

struct DrawSettings {
    int x;
    int y;
    double pointSize;
    bool draw;
    Subspace subspace;
};
