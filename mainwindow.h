#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

extern QString fileName;  //путь к файлу
extern int selectedLead; //Выбранное отведение
extern int firstCount, secondCount; //начало и конец отсчетов <<--->> количество отсчетов
extern QVector<double> dataArray; //массив с данными

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
    void showInfo(QString);
    void showError(QString);
    void on_DrawBtn_clicked();
    void _drawGraphic();
    void on_action_4_triggered();

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
