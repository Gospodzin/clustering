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
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    ui->logBrowser->document()->setMaximumBlockCount(100);

    connect(&compThread, SIGNAL(computed()), this, SLOT(update()));
    connect(&loadThread, SIGNAL(loaded()), this, SLOT(dataLoaded()));

    logging::LOG::out = &logger;
    connect(&logger, SIGNAL(logSignal(QString)), this, SLOT(log(QString)));
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
    stringToSubspace.clear();
    for(auto clusters : compThread.result){
        std::string subspaceStr = DataWriter::write(clusters.first);
        stringToSubspace.emplace(subspaceStr, clusters.first);
        ui->subspaceSelect->addItem(QString::fromStdString(subspaceStr));
    }
    ui->computeButton->setEnabled(true);
    draw(*compThread.data, compThread.sets);
}

void MainWindow::on_computeButton_clicked() {
    try {
        Settings sets = collectSettings();
        ui->computeButton->setEnabled(false);
        compThread.startWithSets(sets, loadThread.data);
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
        ui->odcCheckBox->setEnabled(true);
    } else {
        ui->odcCheckBox->setEnabled(false);
    }
}

void MainWindow::on_subspaceSelect_currentTextChanged(const QString &val) {
    Clusters clusters = compThread.result[stringToSubspace[val.toStdString()]];
    for(Point& p : *compThread.data)
        p.cid = NOISE;
    for(Cluster* cluster : clusters)
        for(Point* p : cluster->points)
            p->cid = cluster->cid;
}

void MainWindow::on_loadButton_clicked() {
    ui->loadButton->setEnabled(false);
    std::string path = ui->dataFileBox->text().toStdString();
    loadThread.startWitPath(path);
}
