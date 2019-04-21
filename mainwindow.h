#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>

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
};

#endif // MAINWINDOW_H
