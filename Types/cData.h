#ifndef CDATA_H
#define CDATA_H

#include <qcustomplot.h>

struct cData
{
    QVector<double> Array_X, Array_Y;
    QColor color_Line = QColor(0,0,0),
           color_Point = QColor(0,0,0),
           color_Frame = QColor(0,0,0);
    enum cType{Line, Point}; cType Type = Line;

    void Clear()
    {
        Array_X.clear();
        Array_Y.clear();
        color_Line = QColor(0,0,0);
        color_Point = QColor(0,0,0);
        color_Frame = QColor(0,0,0);
        Type = Line;
    }
};

#endif
