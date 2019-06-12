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
    void on_action_triggered();
    void newTaskLFHF(QVector<double>);
    void checkHFLF(QVector<double>);
    void newTaskQRS(QVector<double>);
    void drawQRS(QVector<double>, QVector<double>,
                 QVector<double>, QVector<double>, QVector<double>,
                 QVector<double>, QVector<double>);
    void CalculateSomeProc(QVector<double>, QVector<double>,
                           QVector<double>, QVector<double>, QVector<double>,
                           QVector<double>, QVector<double>);
private:
    Ui::MainWindow *ui;

    QString fileName;
    int selectedLead;
    int firstCount, secondCount;

    QRect oldGeometry;
    QPoint dragPosition;
    bool acceptDrag = true;

    bool eventFilter(QObject *object, QEvent *event);
    enum sBorder{topleft, left, bottomleft, bottom,
                 bottomright, right, topright, top, null} Border = null;
    bool StartResize = false;
    QPoint dragStartPosition;
    QRect dragStartGeometry;

    QCPItemText *chartTitle, *Coordinates;
};

#endif // MAINWINDOW_H
