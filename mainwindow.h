#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "calc.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString fileName;

private slots:

    void on_FileBtn_clicked();

    void on_action_2_triggered();

    void on_action_3_triggered();

    void update(int);

    void on_DrawBtn_clicked();

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
