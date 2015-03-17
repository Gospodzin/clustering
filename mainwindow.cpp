#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>

#include <numeric>
#include <thread>

#include "dataio.h"
#include "guilogger.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->logBrowser->document()->setMaximumBlockCount(100);
    logging::LOG::out = new GuiLogger(ui->logBrowser);
}

MainWindow::~MainWindow()
{
    delete ui;
}

measures::MeasureId MainWindow::getMeasureId(Measure measure) {
    switch(measure) {
    case EUCLIDEAN:
        return measures::MeasureId::Euclidean;
    case MANHATTAN:
        return measures::MeasureId::Manhattan;
    default:
        throw -1;
    }
}

void MainWindow::on_computeButton_clicked()
{
    if(compThread.isRunning())
        compThread.terminate();
    else
        compThread.start();
    try {
        Settings sets = collectSettings();

        std::vector<Point>* data = DataLoader(sets.path).load();
        curData = data;

        switch(sets.algorithm) {
        case DBSCAN:
            switch(sets.dataStructure) {
            case BASIC: runDbscan<BasicDataSet>(data, sets); break;
            case TI: runDbscan<TIDataSet>(data, sets); break;
            } break;
        case PREDECON:
            switch(sets.dataStructure) {
            case BASIC: runPredecon<BasicDataSet>(data, sets); break;
            case TI: runPredecon<TIDataSet>(data, sets); break;
            } break;
        case SUBCLU:
            switch(sets.dataStructure) {
            case BASIC: runSubclu<BasicDataSet>(data, sets); break;
            case TI: runSubclu<TIDataSet>(data, sets); break;
            } break;
        }
    } catch(...) {}
}

void MainWindow::draw(std::vector<Point>& data, Settings sets) {
    int xAttr = sets.x;
    int yAttr = sets.y;

    double width = 420;
    double height = 420;

    double radius = sets.pointSize;

    Point refMax = referenceSelectors::max(data);
    Point refMin = referenceSelectors::min(data);

    double xD = xAttr < data[0].size() ? refMax[xAttr] - refMin[xAttr] : 1;
    double yD = yAttr < data[0].size() ? refMax[yAttr] - refMin[yAttr] : 1;

    double x0 = xAttr < data[0].size() ? -refMin[xAttr] : 0;
    double y0 = yAttr < data[0].size() ? -refMin[yAttr] : 0;

    QGraphicsScene* scene = new QGraphicsScene;
    scene->setSceneRect(0,0,width,height);

    ui->graphicsView->setFixedSize(width, height);
    ui->graphicsView->setScene(scene);

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    QPen pen;


    int clustersNo = StatsCollector().collect(data).size();

    for(int i=0; i<data.size();++i) {
        Point& point = data[i];
        double x = xAttr < point.size() ? (point[xAttr]+x0)*width/xD : width/2;
        double y = yAttr < point.size() ? (point[yAttr]+y0)*height/yD : height/2;
        int cid = point.cid;
        int c = cid*0xFFFFFF / clustersNo;
        QColor color(c%0xFF, (c/0xFF)%0xFF, (c/0xFF/0xFF)%0xFF);
        brush.setColor(color);
        pen.setColor(color);

        scene->addEllipse(x-radius, y-radius, radius*2.0, radius*2.0, pen, brush);
    }
}

template <typename T>
void MainWindow::runSubclu(std::vector<Point>* data, Settings sets) {
    long start = clock();
    Subclu<T> subclu(data, sets.eps, sets.mi, sets.odc);
    subclu.compute();
    double totalTime = double(clock() - start) / CLOCKS_PER_SEC;

    std::string statsStr = DataWriter::write(totalTime) + DataWriter::write(StatsCollector().collect(subclu.clustersBySubspace));
    ui->outputBrowser->setText(QString::fromStdString(statsStr));

    DataWriter dw("out.txt");
    std::string output = dw.write(subclu.clustersBySubspace);
    dw.write(statsStr);
    dw.write("-------------------\n");
    dw.write(output);
    if(sets.writeOut) {
        ui->outputBrowser->append("-------------------\n");
        ui->outputBrowser->append(QString::fromStdString(output));
    }

    Subspace subspace(data->front().size());
    std::iota(subspace.begin(), subspace.end(), 0);

    ui->subspaceSelect->clear();
    for(auto clusters : subclu.clustersBySubspace)
        ui->subspaceSelect->addItem(QString::fromStdString(dw.write(clusters.first)));
    ui->subspaceSelect->setCurrentText(QString::fromStdString(dw.write(subspace)));

    Clusters clusters = subclu.clustersBySubspace[subspace];
    for(Point& p : subclu.data)
        p.cid = NOISE;
    for(Cluster* cluster : clusters)
        for(Point* p : cluster->points)
            p->cid = cluster->cid;

    cache = subclu.clustersBySubspace;

    stringToSubspace.clear();
    for(auto clusters : subclu.clustersBySubspace)
        stringToSubspace.emplace(dw.write(clusters.first), clusters.first);

    if(sets.draw) draw(subclu.data, sets);
}

template<typename T>
void MainWindow::runDbscan(std::vector<Point>* data, Settings sets) {
    long start = clock();
    T dataSet(data, getMeasureId(sets.measure), referenceSelectors::max);
    Dbscan<T> dbscan(&dataSet, sets.eps, sets.mi);
    dbscan.compute();
    double totalTime = double(clock() - start) / CLOCKS_PER_SEC;

    std::string statsStr = DataWriter::write(totalTime) + DataWriter::write(StatsCollector().collect(*dataSet.data));
    ui->outputBrowser->setText(QString::fromStdString(statsStr));

    DataWriter dw("out.txt");
    std::string output = dw.write(dbscan.data->data);
    dw.write(statsStr);
    dw.write("-------------------\n");
    dw.write(output);
    if(sets.writeOut) {
        ui->outputBrowser->append("-------------------\n");
        ui->outputBrowser->append(QString::fromStdString(output));
    }
    if(sets.draw) draw(*dbscan.data->data, sets);
}

template<typename T>
void MainWindow::runPredecon(std::vector<Point>* data, Settings sets) {
    long start = clock();
    T dataSet(data, getMeasureId(sets.measure), referenceSelectors::max);
    Predecon<T> predecon(&dataSet, sets.eps, sets.mi, sets.delta, sets.lambda);
    predecon.compute();
    double totalTime = double(clock() - start) / CLOCKS_PER_SEC;

    std::string statsStr = DataWriter::write(totalTime) + DataWriter::write(StatsCollector().collect(*dataSet.data));
    ui->outputBrowser->setText(QString::fromStdString(statsStr));

    DataWriter dw("out.txt");
    std::string output = dw.write(predecon.data->data, predecon.prefDims);
    dw.write(statsStr);
    dw.write("-------------------\n");
    dw.write(output);
    if(sets.writeOut) {
        ui->outputBrowser->append("-------------------\n");
        ui->outputBrowser->append(QString::fromStdString(output));
    }
    if(sets.draw) draw(*predecon.data->data, sets);
}

void MainWindow::on_browseButton_clicked() {
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Select Data"));
    if(!fileNames.empty())
        ui->dataFileBox->setText(fileNames.front());
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
    bool draw = ui->drawCheckBox->isChecked();
    QString pointSize = ui->pointSizeBox->text();
    bool writeOut = ui->writeOutCheckBox->isChecked();
    QString x = ui->xBox->text();
    QString y = ui->yBox->text();
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
    sets.draw = draw;

    tmpD = pointSize.toDouble(&ok);
    if(ok) sets.pointSize = tmpD;
    else {QMessageBox::warning(NULL, "Warning!", "Bad point size value!"); throw -1;}

    sets.writeOut = writeOut;

    tmpI = x.toInt(&ok);
    if(ok) sets.x = tmpI;
    else {QMessageBox::warning(NULL, "Warning!", "Bad mi value!"); throw -1;}
    tmpI = y.toInt(&ok);
    if(ok) sets.y = tmpI;
    else {QMessageBox::warning(NULL, "Warning!", "Bad mi value!"); throw -1;}

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
        ui->subspaceSelect->setEnabled(true);
        ui->odcCheckBox->setEnabled(true);
    } else {
        ui->subspaceSelect->setEnabled(false);
        ui->subspaceSelect->clear();
        ui->odcCheckBox->setEnabled(false);
    }
}

void MainWindow::on_subspaceSelect_currentTextChanged(const QString &val) {
    Clusters clusters = cache[stringToSubspace[val.toStdString()]];
    for(Point& p : *curData)
        p.cid = NOISE;
    for(Cluster* cluster : clusters)
        for(Point* p : cluster->points)
            p->cid = cluster->cid;

    draw(*curData, collectSettings());
}
