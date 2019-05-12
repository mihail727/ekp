#include "mainwindow.h"
#include <QApplication>
#include <QScreen>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow w;
    qRegisterMetaType<QVector<double> >("QVector<double>");

    w.show();

    return app.exec();
}
