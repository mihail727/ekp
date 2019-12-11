#ifndef CQRS_H
#define CQRS_H

#include <QObject>
#include <Types/cData.h>

class cQRS : public QObject
{
    Q_OBJECT
public:

signals:
    void finished();
    void sendValues_for_drawGraphic(QList<cData>);
    void sendValues_for_calculate(QList<cData>);
public slots:
     void doCalc(const QVector<double> &mas, int firstCount, int secondCount);

};

#endif // CQRS_H
