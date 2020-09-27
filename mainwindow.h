#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>

#include <qcustomplot.h>
#include <Controls/datafile.h>
#include <Controls/chartcontrol.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Ui::MainWindow *ui;

    int CurrentAbduction = 0; // Текущее отведение
    double Hertz = 1000; // Герцовка аппарата
    int SamplesCount = 0; // Количество отсчетов во времени

    enum eMethod{ekp, emd, Default}; eMethod currentMethod = ekp;

private slots:
    void showError(QString);

    void GenerateDiagnosis();

    void on_BtnOpenFile_clicked();

    void on_BtnStart_clicked();

    void on_comboBoxAbduction_currentIndexChanged(int index);

    void on_comboBoxTime_currentIndexChanged(int index);

    void on_comboBoxMethod_currentIndexChanged(int index);

private:
    double time = 0;
    double timeFactor = 0.001; // мс, с, мин, часы

    int firstCount = 0,
        secondCount = 0;
    QString Diagnosis = "";

    QRect oldGeometry;
    QPoint dragPosition;
    bool acceptDrag = true;

    bool eventFilter(QObject *object, QEvent *event);

    QCPItemText *chartTitle, *Coordinates;

    void CheckAllFeilds_isNoEmpty(DataFile &dataFile);

    void GenerateDataConfig(QVector<double>& topsQRS, QVector<double>& topsQ,
                            QVector<double>& topsS);
};

#endif // MAINWINDOW_H
