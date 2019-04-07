#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QtCharts/QChartView>

QT_CHARTS_USE_NAMESPACE

class ChartView : public QChartView
{
public:
    ChartView(QChart *chart, QWidget *parent = nullptr);

protected:

private:
    QChart *chart;

};

#endif
