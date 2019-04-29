#include "Widgets/Chartview.h"
#include <QtGui/QMouseEvent>

ChartView::ChartView(QChart *chart, QWidget *parent) :
    QChartView(chart, parent)
{
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setAnimationDuration(400);
}

ChartView::~ChartView()
{

}
