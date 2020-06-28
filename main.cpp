#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QDate expire(2020,12,1);

    if(QDate::currentDate() > expire) {
        return 0;
    }

    QApplication a(argc, argv);

#ifdef Q_OS_WIN
    QApplication::setStyle(QStyleFactory::create("Fusion"));
#endif

    MainWindow w;
    w.show();
    return a.exec();
}
