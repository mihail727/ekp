#include "chartcontrol.h"

ChartControl::ChartControl(QCustomPlot* _chart)
{
    chart = _chart;

    numOfGraph = 0;

    QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
    chart->xAxis->setTicker(fixedTicker);
    chart->yAxis->setTicker(fixedTicker);

    fixedTicker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples);
    fixedTicker->setTickCount(16);

    chart->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    chart->axisRect()->setRangeZoom(Qt::Horizontal);

    chart->rescaleAxes();
    chart->yAxis->setRange(chart->yAxis->range().lower-1000,
                           chart->yAxis->range().upper+1000);
    chart->replot();
}

void ChartControl::AddGraphic(QVector<double> array_X, QVector<double> array_Y, QString lineStyle,
                              QColor colorPoint, QColor colorFrame,
                              QColor colorLine)
{
    chart->addGraph();
    chart->graph(numOfGraph)->setData(array_X, array_Y);
    chart->graph(numOfGraph)->setPen(QPen(colorLine));
    if(lineStyle == "lsLine")
        chart->graph(numOfGraph)->setLineStyle(QCPGraph::lsLine);
    else {
        chart->graph(numOfGraph)->setLineStyle(QCPGraph::lsNone);
        chart->graph(numOfGraph)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,
                                                                  colorFrame,
                                                                  colorPoint, 7));
    }

    numOfGraph++;

    chart->rescaleAxes();
    chart->yAxis->setRange(chart->yAxis->range().lower-300,
                           chart->yAxis->range().upper+300);

    chart->replot();
}

QVector<double> ChartControl::getTimeArray(double hertz, int samplesCount)
{
    QVector<double> mas(samplesCount);

    for(int i = 0; i < mas.length(); i++)
        mas[i] = i * (1 / hertz);

    return mas;
}

void ChartControl::ClearChart()
{
    chart->clearGraphs();
}
