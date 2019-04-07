#include "chartview.h"
#include <QtGui/QMouseEvent>

ChartView::ChartView(QChart *chart, QWidget *parent) :
    QChartView(chart, parent)
{
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->setAnimationDuration(400);
}
