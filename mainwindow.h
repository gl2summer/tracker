#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QWebEngineView>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void getMapDriveRoute(const QString &usrData, const QString &startPlace, const QString &endPlace);
    void getMapDriveRoutes(const QString &usrData, const QString &placeSet, const QString &seq);

public slots:
    void updateMapMousePosition(QString lon, QString lat);
    void updateMapDriveRoute(QString usrData, bool success, QString distances, QString durations);
    void queryMapDriveRoute(QString usrData, QString paths, QString seq);

private slots:
    void updateTotalDistanceAndDuration();
    void addPathToList(QString paths, bool query=false);
    void removeSelectedPathFromList();
    void addPathToListFromFile();

    void on_actionImportPathPoints_triggered();

    void on_actionExportPathDistances_triggered();

    void on_pbAddPathToList_clicked();

    void on_pbImportSites_clicked();

    void on_pbAddPathToListAndQuery_clicked();

private:
    Ui::MainWindow *ui;

    QWebEngineView *mapView;

    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event) override;
};
#endif // MAINWINDOW_H
