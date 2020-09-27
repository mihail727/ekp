#ifndef EMD_H
#define EMD_H

#include <QObject>
#include <Controls/chartcontrol.h>

class cEMD : public QObject
{
    Q_OBJECT
public:
    void doCalc(const QVector<double> &mas, ChartControl& chartControl,
                QVector<double> arrayFormatted);
};

#endif // EMD_H
