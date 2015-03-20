#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "settings.h"
#include "StatsCollector.h"
#include "clustering.h"
#include "computationthread.h"
#include "loaddatathread.h"
#include "guilogger.h"
#include "qcustomplot.h"

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
    void on_loadButton_clicked();
    void on_refreshButton_clicked();
    void on_algorithmSelect_currentTextChanged(const QString &arg1);

    void update();
    void dataLoaded();
    void log(QString msg);
    void plotClick(QMouseEvent* mouseEvent);

private:
    ComputationThread compThread;
    LoadDataThread loadThread;
    GuiLogger logger;
    std::map<std::string, Subspace> stringToSubspace;

    Point* selectedPoint = NULL;
    QCPScatterStyle oldScatterStyle;

    DrawSettings lastDrawSets;

    Ui::MainWindow *ui;

    Settings collectSettings();
    DrawSettings collectDrawSettings();

    void draw(std::vector<Point>& data, DrawSettings sets);
    void updateDataBySubspace(Subspace subspace);
    void updateSubspaceSelect();
    void updateDimensionSelects();
    void updateSelectedPointView();
    void updateSelectedClusterView();
    void selectPoint(QMouseEvent* mouseEvent);
};

#endif // MAINWINDOW_H