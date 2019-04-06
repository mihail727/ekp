#ifndef CALC_H
#define CALC_H

#include "calc.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

class Calc : public QObject
{
    Q_OBJECT

public:
    void doCalc(const QString &, const int &, const int &, const int &);

signals:
    void sendError(QString);
    void drawGraphic(QVector<double>, const double &, const double &);

public slots:

private:
    double minValueOfDataArray;
    double maxValueOfDataArray;
};

#endif // CALC_H
