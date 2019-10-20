#ifndef EMD_H
#define EMD_H

#include <QObject>

class cEMD : public QObject
{
    Q_OBJECT
public:

signals:
    void finished();
    void drawEMD(QVector<double>, QVector<double>);
public slots:
    void doCalc(const QVector<double> &mas, int firstCount, int secondCount);
};

#endif // EMD_H
