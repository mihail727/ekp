#include "emd.h"
#include "QVector"

void cEMD::doCalc(const QVector<double> &mas, int firstCount, int secondCount)
{
    cData Data;
    QList<cData> List;

    for(int i=0; i<mas.size(); i++)
        Data.Array_X.push_back(i);
    Data.Array_Y = mas;
    Data.Type = cData::Line;
//    Data.color_Line = QColor(255,255,255);
//    Data.color_Point = QColor(255,255,255);
//    Data.color_Frame = QColor(255,255,255);
    List.push_back(Data);
    Data.Clear();

    emit draw_graphic(List);
    emit finished();
}
