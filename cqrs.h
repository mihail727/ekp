#ifndef CQRS_H
#define CQRS_H

#include <QObject>

class cQRS : public QObject
{
    Q_OBJECT
public:

signals:
    void finished();
    void sendQRSValues();

public slots:
     void doCalc(const QVector<double> &mas, int selectedLead,
                 int firstCount, int secondCount);

};

#endif // CQRS_H
