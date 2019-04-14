#ifndef HFLF_H
#define HFLF_H

#include <QObject>
#include <mainwindow.h>

class hflf : public QObject
{
    Q_OBJECT

public:
    hflf(QVector<double> &_dataArray, int _Nvl)
    {
        AVal = _dataArray;
        Nvl = _Nvl;
    }

signals:
    void sendArray(QVector<double>);

public slots:
    void doCalc();

private:
    QVector<double> AVal;
    int Nvl;
};

#endif // HFLF_H
