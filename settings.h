#pragma once

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
    bool draw;
    double pointSize;
    bool writeOut;
    int x;
    int y;
    bool odc;
};
