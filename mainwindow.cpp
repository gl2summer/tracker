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

    connect(mapView, &QWebEngineView::loadFinished, [=](){
        releaseMouse();
        releaseKeyboard();
    });

#if defined (Q_OS_WIN)
    mapView->load(QUrl(qApp->applicationDirPath()+ "/map/mapPreview.html"));
#elif defined (Q_OS_DARWIN)
    QByteArray html;
    QFile file(qApp->applicationDirPath()+ "/map/mapPreview_mac.html");
    if(file.open(QIODevice::ReadOnly)) {
        mapView->setHtml(file.readAll());
        file.close();
    } else {
        QMessageBox::warning(this, "error", "map load error!");
    }
#endif

    mapView->show();

    ui->twPathList->setSelectionBehavior(QAbstractItemView::SelectRows);
    QHeaderView *hvPathList = ui->twPathList->horizontalHeader();
    hvPathList->setSectionResizeMode(TWPATHLIST_INDEX_PATH, QHeaderView::Stretch);
    hvPathList->setSectionResizeMode(TWPATHLIST_INDEX_DISTANCE, QHeaderView::ResizeToContents);
    hvPathList->setSectionResizeMode(TWPATHLIST_INDEX_DURATION, QHeaderView::ResizeToContents);
    hvPathList->setSectionResizeMode(TWPATHLIST_INDEX_TRACE, QHeaderView::ResizeToContents);

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

void MainWindow::updateMapDriveRoute(QString usrData, bool hasResult, QString distance, QString duration)
{
    releaseMouse();
    releaseKeyboard();

    qDebug() << usrData << "result:" << hasResult << "distance:" << distance << "duration:" << duration;
    if(usrData.startsWith(TRACK_USR_DATA_TWPATHLIST)) {
        int rowNum = usrData.remove(TRACK_USR_DATA_TWPATHLIST).toInt();
        if(rowNum >= 0) {
            ui->twPathList->item(rowNum, TWPATHLIST_INDEX_DISTANCE)->setText(QString::fromLocal8Bit("%1公里").arg(distance.toInt()/1000.0));
            ui->twPathList->item(rowNum, TWPATHLIST_INDEX_DURATION)->setText(QString::fromLocal8Bit("%1小时").arg(duration.toInt()/3600.0));

            double totalDis = 0;
            double totalDur = 0;
            for(int i=0; i<ui->twPathList->rowCount(); i++) {
                QString dis = ui->twPathList->item(i, TWPATHLIST_INDEX_DISTANCE)->text().remove(QString::fromLocal8Bit("公里"));
                QString dur = ui->twPathList->item(i, TWPATHLIST_INDEX_DURATION)->text().remove(QString::fromLocal8Bit("小时"));

                totalDis += dis.toDouble();
                totalDur += dur.toDouble();
            }
            ui->leTotalDistance->setText(QString::fromLocal8Bit("%1公里").arg(totalDis));
            ui->leTotalDuration->setText(QString::fromLocal8Bit("%1小时").arg(totalDur));
        }
    }
}

void MainWindow::queryMapDriveRoute(QString usrData, QString paths, QString seq)
{
    grabMouse(Qt::WaitCursor);
    grabKeyboard();
    emit getMapDriveRoutes(usrData, paths, seq);
}

void MainWindow::on_actionImportPathPoints_triggered()
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
                aline = file.readLine();
                if(aline.isEmpty()) {
                    break;
                }

                int newRow = ui->twPathList->rowCount();
                ui->twPathList->setRowCount(newRow+1);
                QString rowFlag = QString(TRACK_USR_DATA_TWPATHLIST) + QString::number(newRow);

                QString line = QString(aline).trimmed();
                QTableWidgetItem *itemPath = new QTableWidgetItem(line);
                itemPath->setData(Qt::UserRole, rowFlag);
                ui->twPathList->setItem(newRow, TWPATHLIST_INDEX_PATH, itemPath);

                ui->twPathList->setItem(newRow, TWPATHLIST_INDEX_DISTANCE, new QTableWidgetItem());
                ui->twPathList->setItem(newRow, TWPATHLIST_INDEX_DURATION, new QTableWidgetItem());

                QToolButton *tbTrace = new QToolButton(ui->twPathList);
                tbTrace->setText(QString::fromLocal8Bit("路线规划"));
                tbTrace->setAutoRaise(true);
                //tbTrace->setStyleSheet("*::hover{background-color: lightblue}\n");
                connect(tbTrace, &QToolButton::clicked, [=]() {
                    QString places = ui->twPathList->item(newRow, TWPATHLIST_INDEX_PATH)->text();
                    queryMapDriveRoute(rowFlag, places, QStringLiteral(","));
                });
                ui->twPathList->setCellWidget(newRow, TWPATHLIST_INDEX_TRACE, tbTrace);
            }while(true);

            file.close();
        }
    }
}

void MainWindow::on_actionExportPathDistances_triggered()
{

}
