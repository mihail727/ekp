#ifndef CQRS_H
#define CQRS_H

#include <QObject>
#include <Methods/method.h>

class cQRS : public Method
{
    Q_OBJECT
public:
    void doCalc(const QVector<double> &mas, int firstCount, int secondCount,
                ChartControl& chartControl, QVector<double> arrayFormatted) override;
};

#endif // CQRS_H
