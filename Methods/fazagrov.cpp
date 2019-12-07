#include "fazagrov.h"

void Fazagrov::doCalc(const QVector<double> &mas, int firstCount, int secondCount)
{

    emit draw_graphic(mas); //сюда кинешь вектор для вывода
    emit finished();
}
