#ifndef CQRS_H
#define CQRS_H

#include <QObject>
#include <Controls/chartcontrol.h>

class cQRS : public QObject
{
    Q_OBJECT
public:
    void doCalc(const QVector<double> &mas, int firstCount, int secondCount,
                ChartControl& chartControl, QVector<double> arrayFormatted);
};

#endif // CQRS_H
