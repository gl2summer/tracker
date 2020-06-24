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
    void updateMapDriveRoute(QString usrData, bool hasResult, QString distance, QString duration);
    void queryMapDriveRoute(QString usrData, QString paths, QString seq);

private slots:
    void on_actionImportPathPoints_triggered();

    void on_actionExportPathDistances_triggered();

private:
    Ui::MainWindow *ui;

    QWebEngineView *mapView;
};
#endif // MAINWINDOW_H
