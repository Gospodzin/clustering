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

    void update();

    void dataLoaded();

    void log(QString msg);

    void on_loadButton_clicked();

private:
    ComputationThread compThread;
    LoadDataThread loadThread;
    GuiLogger logger;
    std::map<std::string, Subspace> stringToSubspace;

    Ui::MainWindow *ui;

    Settings collectSettings();

    void draw(std::vector<Point>& data, Settings sets);
};

#endif // MAINWINDOW_H
