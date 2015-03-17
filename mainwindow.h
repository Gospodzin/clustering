#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "settings.h"
#include "StatsCollector.h"
#include "clustering.h"
#include "computationthread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_computeButton_clicked();

    void on_browseButton_clicked();

    void on_algorithmSelect_currentTextChanged(const QString &arg1);

    void on_subspaceSelect_currentTextChanged(const QString &arg1);

private:
    std::map<Subspace, Clusters> cache;
    std::map<std::string, Subspace> stringToSubspace;
    std::vector<Point>* curData;
    ComputationThread compThread;


    Ui::MainWindow *ui;

    Settings collectSettings();

    measures::MeasureId getMeasureId(Measure measure);

    template<typename T>
    void runPredecon(std::vector<Point>* data, Settings sets);

    template<typename T>
    void runDbscan(std::vector<Point>* data, Settings sets);

    template <typename T>
    void runSubclu(std::vector<Point>* data, Settings sets);

    void draw(std::vector<Point>& data, Settings sets);

};

#endif // MAINWINDOW_H
