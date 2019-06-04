#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include "QtCharts/QtCharts"

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
                 QVector<double>, QVector<double>, QVector<double>);
private:
    Ui::MainWindow *ui;

    QString fileName;
    int selectedLead;
    int firstCount, secondCount;

    QRect oldGeometry;
    QPoint dragPosition;
    bool acceptDrag = true;

    bool eventFilter(QObject *object, QEvent *event);
};

#endif // MAINWINDOW_H
