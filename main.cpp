#include "mainwindow.h"

#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QDate expire(2020,12,1);

    if(QDate::currentDate() > expire) {
        return 0;
    }

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
