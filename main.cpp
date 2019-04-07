#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow w;
    w.show();  
    const QRect screen = QApplication::desktop()->screenGeometry();
    w.move( screen.center() - w.rect().center() );
    qRegisterMetaType<QVector<double> >("QVector<double>");

    return app.exec();
}
