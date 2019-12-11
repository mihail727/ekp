#ifndef EMD_H
#define EMD_H

#include <QObject>
#include <Types/cData.h>

class cEMD : public QObject
{
    Q_OBJECT
public:

signals:
    void finished();
    void draw_graphic(QList<cData>);
public slots:
    void doCalc(const QVector<double> &mas, int firstCount, int secondCount);
};

#endif // EMD_H
