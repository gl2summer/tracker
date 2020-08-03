#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "iview.h"
#include "presenter.h"

#include <QMainWindow>

#include <QWebEngineView>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow, public IView
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void getMapDriveRoutes(const QString &usrData, const QString &placeSet, const QString &seq);

public slots:
    void queryMapDriveRoute(QString usrData, QString paths, QString seq);
    void updateMapDriveRoute(QString usrData, QString startPlace, QString endPlace, bool success, QString distance, QString duration);
    void updateMapDriveRoutesTotalResult(QString usrData, bool success, QString totalDistance, QString totalDuration);
    void updateMapMousePosition(QString lon, QString lat);

private slots:
    void updateTotalDistanceAndDuration();
    void addPathToList(QString paths, bool query=false);
    QStringList getPathsFromList();
    void removeSelectedPathFromList();
    void addPathToListFromFile();
    void savePathToListToFile();

    void on_pbAddPathToList_clicked();

    void on_pbImportSites_clicked();

    void on_pbAddPathToListAndQuery_clicked();

    void on_pbExportSites_clicked();

private:
    Ui::MainWindow *ui;

    QWebEngineView *mapView;

    Presenter* presenter;

    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event) override;

    // IView interface
public:
    void showMessage(QString message) override;
};
#endif // MAINWINDOW_H
