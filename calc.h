#ifndef CALC_H
#define CALC_H

#include <QObject>

class Calc : public QObject
{
    Q_OBJECT
public:
    explicit Calc(QObject *parent = nullptr);

signals:

    void send(int);

public slots:

    void doCalc();

};

#endif // CALC_H
