#ifndef EMD_H
#define EMD_H

#include <QObject>
#include <Methods/method.h>

class cEMD : public Method
{
    Q_OBJECT
public:
    void doCalc(const QVector<double> &mas, int firstCount, int secondCount,
                ChartControl& chartControl, QVector<double> arrayFormatted) override;
};

#endif // EMD_H
