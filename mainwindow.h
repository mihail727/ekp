#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>

#include <qcustomplot.h>

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
    void _drawGraphic(QVector<double>);
    void newTaskLFHF(QVector<double>);
    void newTaskEMD(QVector<double>);
    void checkHFLF(QVector<double>);
    void newTaskQRS(QVector<double>);
    void newTaskFazagrov(QVector<double>);
    void drawQRS(QVector<double>, QVector<double>,
                 QVector<double>, QVector<double>,
                 QVector<double>, QVector<double>,
                 QVector<double>, QVector<double>);
    void _drawEMD(QVector<double>, QVector<double>);
    void _drawFazagrov(QVector<double>);
    void CalculateSomeProc(QVector<double>, QVector<double>, QVector<double>);
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();
    void Calc_Diag();

    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;

    QString fileName;
    int selectedLead;
    int firstCount, secondCount;
    QString Diagnosis;

    QRect oldGeometry;
    QPoint dragPosition;
    bool acceptDrag = true;

    enum cMethod{ns, ekp, fazagrov}; cMethod Method;

    bool eventFilter(QObject *object, QEvent *event);

    QCPItemText *chartTitle, *Coordinates;
};

#endif // MAINWINDOW_H
