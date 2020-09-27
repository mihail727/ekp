#ifndef ABSTRACTMETHOD_H
#define ABSTRACTMETHOD_H

#include <QObject>
#include <Controls/chartcontrol.h>

class Method : public QObject
{
    Q_OBJECT
public:
    virtual void doCalc(const QVector<double> &mas, int firstCount, int secondCount,
                        ChartControl& chartControl, QVector<double> arrayFormatted) = 0;

signals:

};

#endif // ABSTRACTMETHOD_H
