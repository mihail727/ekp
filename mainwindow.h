#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include "QtCharts/QtCharts"

#include <Widgets/Chartview.h>
#include <Widgets/Chart.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_FileBtn_clicked();
    void on_action_2_triggered();
    void on_action_3_triggered();
    void showError(QString);
    void on_DrawBtn_clicked();
    void _drawGraphic(QVector<double>, const double &, const double &);
    void on_action_4_triggered();
    void on_action_5_triggered();
    void on_action_triggered();
    void newTaskLFHF(QVector<double>);
    void checkHFLF(QVector<double>);
    void newTaskQRS(QVector<double>);
    void drawQRS(QVector<double>);

private:
    Ui::MainWindow *ui;
    ChartView *ChartView1;
    Chart *Chart1;
    QLineSeries *Series1;
    QLineSeries *Series2;

    Chart *Chart2;
    QLineSeries *LineSeries1;
    QLineSeries *LineSeries2;
    ChartView *ChartView2;

    QValueAxis *axisX;
    QValueAxis *axisY;
    QCategoryAxis *axisZero;
    QValueAxis *axisX2;
    QValueAxis *axisY2;
    QCategoryAxis *axisZero2;

    QString fileName;
    int selectedLead;
    int firstCount, secondCount;
};

#endif // MAINWINDOW_H
