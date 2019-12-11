#include "fazagrov.h"
#include "Types/cData.h"

void Fazagrov::doCalc(const QVector<double> &mas, int firstCount, int secondCount)
{
    QList<cData> List;

    for(int i=0; i<mas.size(); i++)
        Data.Array_X.push_back(i);
    Data.Array_Y = mas;
    Data.Type = cData::Point;
    Data.color_Line = QColor(255,0,0);
    Data.color_Point = QColor(255,255,0);
    Data.color_Frame = QColor(255,255,255);
    Data.Clear();

    List.push_back(Data);

    emit draw_graphic(List);
    emit finished();
}
