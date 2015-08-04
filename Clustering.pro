#-------------------------------------------------
#
# Project created by QtCreator 2015-02-05T21:57:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = Clustering
TEMPLATE = app

CONFIG += static

SOURCES += main.cpp \
        mainwindow.cpp \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/AlgorithmTester.cpp \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/DataLoader.cpp \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/DataWriter.cpp \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/logging.cpp \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/measures.cpp \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/Point.cpp \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/referenceSelectors.cpp \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/libpca/pca.cpp \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/libpca/utils.cpp \
    qcustomplot/qcustomplot.cpp

FORMS    += mainwindow.ui

HEADERS +=     mainwindow.h \
             settings.h \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/utils.h \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/algorithms.h \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/AlgorithmTester.h \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/BasicDataSet.h \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/Cluster.h \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/clustering.h \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/dataio.h \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/DataLoader.h \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/DataSet.h \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/datasets.h \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/DataWriter.h \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/Dbscan.h \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/logging.h \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/measures.h \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/Point.h \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/Subspace.h \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/Predecon.h \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/Qscan.h \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/referenceSelectors.h \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/Subclu.h \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/SubcluUI.h \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/TIDataSet.h \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/PLDataSet.h \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/SegTreeDataSet.h \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/VaFileDataSet.h \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/UI.h \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/Data.h \
    ../../../Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources/libpca/pca.h \
    guilogger.h \
    computationthread.h \
    loaddatathread.h \
    qcustomplot/qcustomplot.h \
    datastatsthread.h \
    pcathread.h

INCLUDEPATH += C:/Utils/boost_1_55_0 \
            C:/Users/Gospo/Desktop/Praca_Magisterska/impl/Predecon/Predecon/Sources \
            C:/Users/Gospo/Documents/Qt/Clustering/qcustomplot \
            C:/Utils/Eigen \
            C:/Utils/armadillo/include



LIBS += C:/Users/Gospo/Desktop/Praca_Magisterska/impl/Predecon/Predecon/lib/cbia.lib.blas.dyn.rel.x86.12.lib \
        C:/Users/Gospo/Desktop/Praca_Magisterska/impl/Predecon/Predecon/lib/cbia.lib.lapack.dyn.rel.x86.12.lib
