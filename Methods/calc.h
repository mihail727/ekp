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
    void finished(QVector<double>);

public slots:
    void doCalc(QString &fileName, int &selectedLead,
                int &firstCount, int &secondCount);

private:
    QVector<double> dataArray;
};

#endif // CALC_H
