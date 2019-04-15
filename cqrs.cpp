#include "cqrs.h"

/*
 * mas - массив с иходными данными; selectedLead - выбранное отведение;
 * first - second Count - начальное - конечное значение отсчетов;
 * Тебе нужно сформировать массив с данными но уже QRS
 *
 * { И ещё ГФ сказал что ты в своем методе находишь макс точки
 * если да то засунь его сюда тоже и скажи какие переменные под эти
 * значения выделил! }
*/

void cQRS::doCalc(const QVector<double> &mas, int selectedLead,
                  int firstCount, int secondCount)
{
    /* ПИСАТЬ СЮДА ;) */

    //emit sendQRSValues();
    emit finished();
}
