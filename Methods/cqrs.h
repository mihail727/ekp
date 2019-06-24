#ifndef CQRS_H
#define CQRS_H

#include <QObject>

class cQRS : public QObject
{
    Q_OBJECT
public:

signals:
    void finished();
    void sendValues_for_drawGraphic(QVector<double>, QVector<double>,
                       QVector<double>, QVector<double>, QVector<double>,
                       QVector<double>, QVector<double>);
    void sendValues_for_calculate(QVector<double>, QVector<double>, QVector<double>);
public slots:
     void doCalc(const QVector<double> &mas, int firstCount, int secondCount);

};

#endif // CQRS_H
