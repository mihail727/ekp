#ifndef FAZAGROV_H
#define FAZAGROV_H

#include <QObject>
#include <qcustomplot.h>
#include <Types/cData.h>

class Fazagrov : public QObject
{
    Q_OBJECT
private:

public:
    cData Data;

signals:
    void finished();
    void draw_graphic(QList<cData>);

public slots:
    void doCalc(const QVector<double> &mas, int firstCount, int secondCount);
};

#endif // FAZAGROV_H
