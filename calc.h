#ifndef CALC_H
#define CALC_H

#include "calc.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

class Calc : public QObject
{
    Q_OBJECT

public:

signals:
    void sendError(QString);
    void drawGraphic(QVector<double>, const double &, const double &);

public slots:
    void doCalc(QString fileName, int selectedLead, int firstCount, int secondCount);

private:
    double minValueOfDataArray;
    double maxValueOfDataArray;
};

#endif // CALC_H
