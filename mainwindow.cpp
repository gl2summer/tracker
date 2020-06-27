#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QDebug>
#include <QWebEnginePage>
#include <QWebChannel>
#include <QWebEngineScriptCollection>
#include <QFileDialog>
#include <QTableWidgetItem>
#include <QToolButton>
#include <QClipboard>
#include <QStatusBar>
#include <QMessageBox>
#include <QKeyEvent>

#define TWPATHLIST_INDEX_PATH       0
#define TWPATHLIST_INDEX_DISTANCE   1
#define TWPATHLIST_INDEX_DURATION   2
#define TWPATHLIST_INDEX_TRACE      3

#define TRACK_USR_DATA_TWPATHLIST   "from_twPathList:"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mapView = new QWebEngineView(ui->wdMap);
    ui->wdMap->layout()->addWidget(mapView);

    QWebChannel *mapChannel = new QWebChannel(ui->wdMap);
    mapChannel->registerObject(QStringLiteral("Mainwindow"), this);
    mapView->page()->setWebChannel(mapChannel);

    connect(mapView, &QWebEngineView::loadStarted, [=](){
        grabMouse(Qt::WaitCursor);
        grabKeyboard();
    });

    connect(mapView, &QWebEngineView::loadFinished, [=](bool ok){
        releaseMouse();
        releaseKeyboard();

        if(!ok) {
            mapView->reload();
        }
    });

#if defined (Q_OS_WIN)
    mapView->load(QUrl(qApp->applicationDirPath()+ "/map/mapPreview.html"));
#elif defined (Q_OS_DARWIN)
    mapView->load(QUrl("file:///" + qApp->applicationDirPath()+ "/map/mapPreview.html"));
#endif
    mapView->show();

    ui->twPathList->setSelectionBehavior(QAbstractItemView::SelectRows);
    QHeaderView *hvPathList = ui->twPathList->horizontalHeader();
    hvPathList->setSectionResizeMode(TWPATHLIST_INDEX_PATH, QHeaderView::Stretch);
    hvPathList->setSectionResizeMode(TWPATHLIST_INDEX_DISTANCE, QHeaderView::ResizeToContents);
    hvPathList->setSectionResizeMode(TWPATHLIST_INDEX_DURATION, QHeaderView::ResizeToContents);
    hvPathList->setSectionResizeMode(TWPATHLIST_INDEX_TRACE, QHeaderView::ResizeToContents);
    ui->twPathList->installEventFilter(this);
    connect(ui->twPathList, &QTableWidget::cellChanged, this, &MainWindow::updateTotalDistanceAndDuration);

    connect(ui->pbCopyTotal, &QPushButton::clicked, [=]() {
        QString distanceAndDuration = QString::fromLocal8Bit("总共：%1,%2").arg(ui->leTotalDistance->text()).arg(ui->leTotalDuration->text());

        QClipboard *clipBoard = QApplication::clipboard();
        clipBoard->setText(distanceAndDuration);
    });

    QStatusBar *sbar = statusBar();

    QLabel *locationLabel = new QLabel(this);
    locationLabel->setObjectName("location");
    locationLabel->setAlignment(Qt::AlignHCenter);
    sbar->addWidget(locationLabel);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateMapMousePosition(QString lon, QString lat)
{
    //qDebug() << "position: " << lon << lat;
    QStatusBar *sbar = statusBar();
    QLabel *locationLabel = sbar->findChild<QLabel *>("location");
    if(locationLabel != nullptr) {
        locationLabel->setText(lon + ',' + lat);
    }
}

void MainWindow::updateTotalDistanceAndDuration()
{
    double totalDis = 0;
    double totalDur = 0;
    for(int i=0; i<ui->twPathList->rowCount(); i++) {
        QTableWidgetItem *disItem = ui->twPathList->item(i, TWPATHLIST_INDEX_DISTANCE);
        QTableWidgetItem *durItem = ui->twPathList->item(i, TWPATHLIST_INDEX_DURATION);
        totalDis += (disItem != nullptr) ? disItem->text().remove(QString::fromLocal8Bit("公里")).toDouble() : 0.0;
        totalDur += (durItem != nullptr) ? durItem->text().remove(QString::fromLocal8Bit("小时")).toDouble() : 0.0;
    }
    ui->leTotalDistance->setText(QString::fromLocal8Bit("%1公里").arg(totalDis));
    ui->leTotalDuration->setText(QString::fromLocal8Bit("%1小时").arg(totalDur));
}

void MainWindow::updateMapDriveRoute(QString usrData, bool success, QString distances, QString durations)
{
    releaseMouse();
    releaseKeyboard();

    qDebug() << usrData << "result:" << success << "distance:" << distances << "duration:" << durations;


    if(usrData.startsWith(TRACK_USR_DATA_TWPATHLIST)) {
        if(!success) {
            QMessageBox::warning(this, tr("ERROR"), tr("路线规划出错"));
            return ;
        }
        int rowNum = usrData.remove(TRACK_USR_DATA_TWPATHLIST).toInt();
        if(rowNum >= 0) {
            QTableWidgetItem *disItem = ui->twPathList->item(rowNum, TWPATHLIST_INDEX_DISTANCE);
            disItem->setToolTip(distances);

            double totalDistance = 0.0;
            QStringList distancesList = distances.split(',');
            qDebug() << "distancesList:" << distancesList;
            for(QString dis : distancesList) {
                totalDistance += dis.toDouble();
            }
            disItem->setText(QString::fromLocal8Bit("%1公里").arg(totalDistance/1000.0));

            QTableWidgetItem *durItem = ui->twPathList->item(rowNum, TWPATHLIST_INDEX_DURATION);
            durItem->setToolTip(durations);

            double totalDuration = 0.0;
            QStringList durationList = durations.split(',');
            qDebug() << "durationList:" << durationList;
            for(QString dur : durationList) {
                totalDuration += dur.toDouble();
            }

            durItem->setText(QString::fromLocal8Bit("%1小时").arg(totalDuration/3600.0));
        }
    }
}

void MainWindow::queryMapDriveRoute(QString usrData, QString paths, QString seq)
{
    grabMouse(Qt::WaitCursor);
    grabKeyboard();
    emit getMapDriveRoutes(usrData, paths, seq);
}

void MainWindow::addPathToList(QString paths, bool query)
{
    if(paths.isEmpty()) {
        return ;
    }

    int newRow = ui->twPathList->rowCount();
    ui->twPathList->setRowCount(newRow+1);
    QString rowFlag = QString(TRACK_USR_DATA_TWPATHLIST) + QString::number(newRow);

    QString line = QString(paths).trimmed();
    QTableWidgetItem *itemPath = new QTableWidgetItem(line);
    itemPath->setData(Qt::UserRole, rowFlag);
    ui->twPathList->setItem(newRow, TWPATHLIST_INDEX_PATH, itemPath);

    ui->twPathList->setItem(newRow, TWPATHLIST_INDEX_DISTANCE, new QTableWidgetItem());
    ui->twPathList->setItem(newRow, TWPATHLIST_INDEX_DURATION, new QTableWidgetItem());

    QToolButton *tbTrace = new QToolButton(ui->twPathList);
    tbTrace->setText(QString::fromLocal8Bit("规划路线"));
    tbTrace->setAutoRaise(true);
    //tbTrace->setStyleSheet("*::hover{background-color: lightblue}\n");
    connect(tbTrace, &QToolButton::clicked, [=]() {
        QString places = ui->twPathList->item(newRow, TWPATHLIST_INDEX_PATH)->text();
        queryMapDriveRoute(rowFlag, places, QStringLiteral(","));
    });
    ui->twPathList->setCellWidget(newRow, TWPATHLIST_INDEX_TRACE, tbTrace);

    if(query) {
        tbTrace->animateClick();
    }
}

void MainWindow::removeSelectedPathFromList()
{
    for(int i=ui->twPathList->rowCount()-1; i>=0; --i) {
        if(ui->twPathList->item(i,0)->isSelected()) {
            ui->twPathList->removeRow(i);
            emit ui->twPathList->cellChanged(i, 0);
        }
    }
}

void MainWindow::addPathToListFromFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open"));
    if(!fileName.isEmpty())
    {
        QFile file(fileName);
        if(file.open(QIODevice::ReadOnly)) {

            ui->twPathList->clearContents();
            ui->twPathList->setRowCount(0);

            file.seek(0);
            QByteArray aline;
            do{
                addPathToList(file.readLine(), false);
            }while(file.pos() < file.size());

            file.close();
        }
    }
}
void MainWindow::on_actionImportPathPoints_triggered()
{
    addPathToListFromFile();
}

void MainWindow::on_actionExportPathDistances_triggered()
{

}

void MainWindow::on_pbAddPathToList_clicked()
{
    QString sites = ui->teTempPaths->toPlainText();
    if(sites.isEmpty()) {
        return ;
    }

    sites = sites.trimmed();
    sites.replace(QRegExp("(\r|\n)+"), ",");

    addPathToList(sites, false);
}

void MainWindow::on_pbAddPathToListAndQuery_clicked()
{
    QString sites = ui->teTempPaths->toPlainText();
    if(sites.isEmpty()) {
        return ;
    }

    sites = sites.trimmed();
    sites.replace(QRegExp("(\r|\n)+"), ",");

    addPathToList(sites, true);
}

void MainWindow::on_pbImportSites_clicked()
{
    addPathToListFromFile();
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if((watched == ui->twPathList) && (event->type() == QEvent::KeyRelease)) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        int keyValue = keyEvent->key();
        if((keyValue == Qt::Key_Backspace) || (keyValue == Qt::Key_Delete)) {
            removeSelectedPathFromList();
            return true;
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

