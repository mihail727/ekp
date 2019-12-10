#include "fazagrov.h"

void Fazagrov::doCalc(const QVector<double> &mas, int firstCount, int secondCount)
{
    QList<QVector<double>> List;
    List.push_back(mas);


    emit draw_graphic(List);
    emit finished();
}
