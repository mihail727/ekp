#ifndef CHARTCONTROL_H
#define CHARTCONTROL_H

#include <QVector>
#include <QColor>
#include <qcustomplot.h>
#include "mainwindow.h"

class ChartControl : public QObject
{
    Q_OBJECT
public:
    QVector<double> topsQRS, topsQ, topsS;

    explicit ChartControl(QCustomPlot* _chart);
    void AddGraphic(QVector<double> array_X, QVector<double> array_Y, QString lineStyle = "lsNone",
                    QColor colorPoint = QColor(247,247,247),
                    QColor colorFrame = QColor(166,0,0),
                    QColor colorLine = QColor(166,0,0));
    QVector<double> getTimeArray(double hertz, int samplesCount);
    void ClearChart();
signals:
    void on_MountChart(QCustomPlot& chart);
    void on_UpdateGraphs();

private:
    QCustomPlot* chart;
    int numOfGraph = 0;
};

#endif // CHARTCONTROL_H
