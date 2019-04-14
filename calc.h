#ifndef CALC_H
#define CALC_H

#include "mainwindow.h"
#include "ui_mainwindow.h"

class Calc : public QObject
{
    Q_OBJECT

public:
    QString fileName;
    int selectedLead;
    int secondCount, firstCount;

signals:
    void sendError(QString);
    void drawGraphic(QVector<double>, const double &, const double &);
    void finished(QVector<double>);

public slots:
    void doCalc();

private:
    double minValueOfDataArray;
    double maxValueOfDataArray;
    QVector<double> dataArray;
};

#endif // CALC_H
