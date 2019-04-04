#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
/*///////////////////////////////////////////////////////////////////*/
                    /*ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ*/
/*///////////////////////////////////////////////////////////////////*/
extern QString fileName;
extern int selectedLead;
extern int firstCount, secondCount;
extern QVector<double> dataArray;
extern double minValueOfDataArray, maxValueOfDataArray;
extern double maxValueOfCount;
extern QFile dataFile;
/*///////////////////////////////////////////////////////////////////*/

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
    void _drawGraphic();
    void on_action_4_triggered();
    void on_action_5_triggered();

    void on_action_triggered();

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
