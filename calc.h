#ifndef CALC_H
#define CALC_H

#include "calc.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QMessageBox>

class Calc : public QObject
{
    Q_OBJECT

public:

signals:
    void send(QString);
    void sendError(QString);
    void drawGraphic();

public slots:
    void doCalc();

private:

};

#endif // CALC_H
