#ifndef HFLF_H
#define HFLF_H

#include <QObject>
#include <mainwindow.h>

class hflf : public QObject
{
    Q_OBJECT

public:

signals:
    void sendArray(QVector<double>);
    void finished();

public slots:
    void doCalc(const QVector<double> &AVal, int Nvl);

private:

};

#endif // HFLF_H
