#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>

#include <qcustomplot.h>
#include <Types/cData.h>

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
    void on_action_2_triggered();
    void showError(QString);
    void newTaskEMD(QVector<double>);
    void newTaskQRS(QVector<double>);
    void newTaskFazagrov(QVector<double>);
    void draw_graphic(QList<cData>);
    void CalculateSomeProc(QList<cData>);
    void on_pushButton_clicked();
    void start_Calc();

    void on_pushButton_2_clicked();
    void Calc_Diag();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;

    QString fileName;
    int selectedLead;
    int firstCount, secondCount;
    QString Diagnosis;

    QRect oldGeometry;
    QPoint dragPosition;
    bool acceptDrag = true;

    enum cMethod{ns, ekp, fazagrov, emd}; cMethod Method = ekp;

    bool eventFilter(QObject *object, QEvent *event);

    QCPItemText *chartTitle, *Coordinates;
};

#endif // MAINWINDOW_H
