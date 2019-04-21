#ifndef CQRS_H
#define CQRS_H

#include <QObject>

class cQRS : public QObject
{
    Q_OBJECT
public:

signals:
    void finished();
    void sendQRSValues(QVector<double>);

public slots:
     void doCalc(const QVector<double> &mas, int firstCount, int secondCount);

};

#endif // CQRS_H
