#ifndef FAZAGROV_H
#define FAZAGROV_H

#include <QObject>

class Fazagrov : public QObject
{
    Q_OBJECT
public:

signals:
    void finished();
    void draw_graphic(QList<QVector<double>>);

public slots:
    void doCalc(const QVector<double> &mas, int firstCount, int secondCount);
};

#endif // FAZAGROV_H
