#include "mainwindow.h"
#include <QApplication>
#include <QScreen>

int main(int argc, char *argv[])
{
    //QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    //QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    //qputenv("QT_SCALE_FACTOR", "1.5");
    QApplication app(argc, argv);

    MainWindow w;
    qRegisterMetaType<QVector<double> >("QVector<double>");
    qRegisterMetaType<QList<QVector<double>> >("QList<QVector<double>>");

    w.show();

    return app.exec();
}
