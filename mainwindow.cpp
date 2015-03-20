#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>

#include <numeric>
#include <thread>
#include <sstream>

#include "dataio.h"
#include "guilogger.h"

#include "utils.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    ui->logBrowser->document()->setMaximumBlockCount(100);
    logging::LOG::out = &logger;

    connect(&compThread, SIGNAL(computed()), this, SLOT(update()));
    connect(&loadThread, SIGNAL(loaded()), this, SLOT(dataLoaded()));
    connect(&logger, SIGNAL(logSignal(QString)), this, SLOT(log(QString)));
    connect(ui->plotView, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(plotClick(QMouseEvent*)));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::dataLoaded() {
    ui->loadButton->setEnabled(true);
    ui->computeButton->setEnabled(true);
}

void MainWindow::log(QString msg) {
    ui->logBrowser->append(msg);
}

void MainWindow::update() {
    ui->computeButton->setEnabled(true);
    ui->refreshButton->setEnabled(true);
    updateDimensionSelects();
    updateSubspaceSelect();
    DrawSettings sets = collectDrawSettings();
    updateDataBySubspace(sets.subspace);
    draw(*compThread.data, sets);
}

void MainWindow::updateSubspaceSelect() {
    stringToSubspace.clear();
    ui->subspaceSelect->clear();
    for(auto clusters : compThread.result){
        std::string subspaceStr = DataWriter::write(clusters.first);
        stringToSubspace.emplace(subspaceStr, clusters.first);
        ui->subspaceSelect->addItem(QString::fromStdString(subspaceStr));
    }
}

void MainWindow::updateDimensionSelects() {
    auto x = ui->xSelect;
    auto y = ui->ySelect;

    x->clear();
    y->clear();

    Subspace attrs = utils::attrsFromData(compThread.data.get());

    x->addItem("-");
    y->addItem("-");

    for(auto attr : attrs) {
        x->addItem(QString::number(attr));
        y->addItem(QString::number(attr));
    }

    x->setCurrentText("0");
    y->setCurrentText("1");
}

void MainWindow::on_computeButton_clicked() {
    try {
        Settings sets = collectSettings();
        ui->computeButton->setEnabled(false);
        compThread.startWithSets(sets, loadThread.data);
    } catch(...) {}
}

void MainWindow::plotClick(QMouseEvent* mouseEvent) {
    selectPoint(mouseEvent);
    updateSelectedPointView();
    updateSelectedClusterView();
}

void MainWindow::selectPoint(QMouseEvent* mouseEvent) {
    if(!compThread.data) return;

    QCustomPlot* plot = ui->plotView;

    double x = plot->xAxis->pixelToCoord(mouseEvent->x());
    double y = plot->yAxis->pixelToCoord(mouseEvent->y());

    int xAttr = lastDrawSets.x;
    int yAttr = lastDrawSets.y;

    double maxXDist = plot->xAxis->pixelToCoord(lastDrawSets.pointSize + 5) - plot->xAxis->pixelToCoord(0);
    double maxYDist = plot->yAxis->pixelToCoord(0)-plot->yAxis->pixelToCoord(lastDrawSets.pointSize + 5);

    Point* minP = NULL;
    double minDist;
    for(int i = 0; i < compThread.data->size(); ++i) {
        Point* p = &compThread.data->at(i);

        double xx = p->at(xAttr);
        double yy = p->at(yAttr);

        double xDist = std::abs(xx-x);
        double yDist = std::abs(yy-y);

        if(xDist <= maxXDist && yDist <= maxYDist) {
            double dist = xDist*xDist + yDist*yDist;

            if(minP == NULL) minP = p;
            else if(dist < minDist) {
                minP = p;
                minDist = dist;
            }
        }
    }

    if(selectedPoint != NULL) {
        plot->graph(selectedPoint->cid)->setScatterStyle(oldScatterStyle);
        selectedPoint = NULL;
    }

    if(minP != NULL) {
        std::stringstream ss;
        ss<<"Select -> X:" <<minP->at(xAttr) << " Y:" << minP->at(yAttr) << " CID:" << minP->cid;
        LOG(ss.str());

        selectedPoint = minP;
        oldScatterStyle = plot->graph(minP->cid)->scatterStyle();

        plot->graph(minP->cid)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::red, oldScatterStyle.size()));
        plot->replot();
    }
}

void MainWindow::updateSelectedClusterView() {
    if(selectedPoint == NULL) {
        ui->selectedClusterView->clearContents();
    } else {
        int cid = selectedPoint->cid;
        Clusters clusters = compThread.result[lastDrawSets.subspace];
        Cluster* cluster = cid == NOISE ? NULL : clusters[cid];
        ui->selectedClusterView->setItem(0, 0, new QTableWidgetItem(cid == NOISE ? "N" : QString::number(cid)));
        ui->selectedClusterView->setItem(1, 0, new QTableWidgetItem(QString::number( cid == NOISE ? utils::countNoise(clusters, compThread.data.get()) : cluster->points.size())));
    }
}

void MainWindow::updateSelectedPointView() {
    if(selectedPoint == NULL) {
        ui->selectedPointView->clearContents();
        ui->selectedPointView->setColumnCount(0);
    } else {
        Subspace args = utils::attrsFromData(compThread.data.get());
        ui->selectedPointView->setColumnCount(args.size());
        for(int i = 0; i < args.size(); ++i) {
            ui->selectedPointView->setItem(0, i, new QTableWidgetItem(QString::number(args[i])));
            ui->selectedPointView->setItem(1, i, new QTableWidgetItem(QString::number(selectedPoint->at(i))));
        }
    }
}

void MainWindow::draw(std::vector<Point>& data, DrawSettings sets) {
    QCustomPlot* plot = ui->plotView;

    selectedPoint = NULL;
    lastDrawSets = sets;
    plot->clearGraphs();

    int xAttr = sets.x;
    int yAttr = sets.y;

    double pointSize = sets.pointSize;

    int clustersNo = StatsCollector().collect(data).size();

    std::vector<QVector<double>> x(clustersNo + 1);
    std::vector<QVector<double>> y(clustersNo + 1);

    for(Point& p : data) {
        x[p.cid].push_back(xAttr < 0 ? 0 : p[xAttr]);
        y[p.cid].push_back(yAttr < 0 ? 0 : p[yAttr]);
    }

    for(int cid=0; cid < clustersNo + 1;++cid) {
        int c = cid*0xFFFFFF / (clustersNo + 1);
        QColor color(c%0xFF, (c/0xFF)%0xFF, (c/0xFF/0xFF)%0xFF);

        plot->addGraph();
        plot->graph(cid)->setData(x[cid], y[cid]);
        plot->graph(cid)->setLineStyle(QCPGraph::lsNone);
        plot->graph(cid)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, color, pointSize));
    }

    plot->xAxis->setLabel("x");
    plot->yAxis->setLabel("y");
    plot->rescaleAxes();

    const double rangePadding = 0.03;

    QCPRange xRange = plot->xAxis->range();
    double xPadding = rangePadding*(xRange.upper - xRange.lower);
    plot->xAxis->setRange(xRange.lower - xPadding, xRange.upper + xPadding);

    QCPRange yRange = plot->yAxis->range();
    double yPadding = rangePadding*(yRange.upper - yRange.lower);
    plot->yAxis->setRange(yRange.lower - yPadding, yRange.upper + yPadding);

    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables );

    plot->replot();
 }

void MainWindow::on_browseButton_clicked() {
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Select Data"));
    if(!fileNames.empty())
        ui->dataFileBox->setText(fileNames.front());
}

DrawSettings MainWindow::collectDrawSettings()  {
    DrawSettings sets;

    QString x = ui->xSelect->currentText();
    QString y = ui->ySelect->currentText();
    QString pointSize = ui->pointSizeBox->text();
    bool draw = ui->drawCheckBox->isChecked();
    QString subspace = ui->subspaceSelect->currentText();

    bool ok;
    int tmpI = x.toInt(&ok);
    if(ok) sets.x = tmpI; else x = -1;

    tmpI = y.toInt(&ok);
    if(ok) sets.y = tmpI; else y = -1;

    double tmpD = pointSize.toDouble(&ok);
    if(ok) sets.pointSize = tmpD;
    else {QMessageBox::warning(NULL, "Warning!", "Bad point size value!"); throw -1;}

    sets.draw = draw;

    sets.subspace = stringToSubspace[subspace.toStdString()];

    return sets;
}


Settings MainWindow::collectSettings() {
    std::string algorithm = ui->algorithmSelect->currentText().toStdString();
    std::string dataStructure = ui->dataStructureSelect->currentText().toStdString();
    std::string measure = ui->measureSelect->currentText().toStdString();
    QString eps = ui->epsBox->text();
    QString mi = ui->miBox->text();
    QString delta = ui->deltaBox->text();
    QString lambda = ui->lambdaBox->text();
    std::string path = ui->dataFileBox->text().toStdString();
    bool writeOut = ui->writeOutCheckBox->isChecked();
    bool odc = ui->odcCheckBox->isChecked();

    Settings sets;

    if(algorithm == "PREDECON") sets.algorithm = Algorithm::PREDECON;
    else if(algorithm == "DBSCAN") sets.algorithm = Algorithm::DBSCAN;
    else if(algorithm == "SUBCLU") sets.algorithm = Algorithm::SUBCLU;
    else {QMessageBox::warning(NULL, "Warning!", "No such algorithm!"); throw -1;}

    if(dataStructure == "TI") sets.dataStructure = DataStructure::TI;
    else if(dataStructure == "BASIC") sets.dataStructure = DataStructure::BASIC;
    else {QMessageBox::warning(NULL, "Warning!", "No such data structure!"); throw -1;}

    if(measure == "EUCLIDEAN") sets.measure = Measure::EUCLIDEAN;
    else if(measure == "MANHATTAN") sets.measure = Measure::MANHATTAN;
    else {QMessageBox::warning(NULL, "Warning!", "No such measure!"); throw -1;}

    bool ok;
    double tmpD = eps.toDouble(&ok);
    if(ok) sets.eps = tmpD;
    else {QMessageBox::warning(NULL, "Warning!", "Bad eps value!"); throw -1;}
    int tmpI = mi.toInt(&ok);
    if(ok) sets.mi = tmpI;
    else {QMessageBox::warning(NULL, "Warning!", "Bad mi value!"); throw -1;}

    if(sets.algorithm == PREDECON) {
        tmpD = delta.toDouble(&ok);
        if(ok) sets.delta = tmpD;
        else {QMessageBox::warning(NULL, "Warning!", "Bad delta value!"); throw -1;}
        tmpI = lambda.toInt(&ok);
        if(ok) sets.lambda = tmpI;
        else {QMessageBox::warning(NULL, "Warning!", "Bad lambda value!"); throw -1;}
    }

    sets.path = path;

    sets.writeOut = writeOut;

    sets.odc = odc;

    return sets;
}

void MainWindow::on_algorithmSelect_currentTextChanged(const QString &val)
{
    if(val.toStdString() == "PREDECON") {
        ui->deltaLabel->setEnabled(true);
        ui->deltaBox->setEnabled(true);
        ui->lambdaLabel->setEnabled(true);
        ui->lambdaBox->setEnabled(true);
    } else {
        ui->deltaLabel->setEnabled(false);
        ui->deltaBox->setEnabled(false);
        ui->lambdaLabel->setEnabled(false);
        ui->lambdaBox->setEnabled(false);
    }

    if(val.toStdString() == "SUBCLU") {
        ui->odcCheckBox->setEnabled(true);
    } else {
        ui->odcCheckBox->setEnabled(false);
    }
}

void MainWindow::on_loadButton_clicked() {
    ui->loadButton->setEnabled(false);
    std::string path = ui->dataFileBox->text().toStdString();
    loadThread.startWitPath(path);
}

void MainWindow::on_refreshButton_clicked() {
    DrawSettings sets = collectDrawSettings();
    updateDataBySubspace(sets.subspace);
    draw(*compThread.data, sets);
}

void MainWindow::updateDataBySubspace(Subspace subspace) {
    Clusters clusters = compThread.result[subspace];
    for(Point& p : *compThread.data)
        p.cid = NOISE;
    for(auto cluster : clusters)
        for(Point* p : cluster.second->points)
            p->cid = cluster.first;
}
