#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QPixmap"
#include "QtCharts/QtCharts"

#include <hflf.h>
#include <calc.h>
#include <chartview.h>
#include <chart.h>
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
        /*ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ И ОБЬЕКТЫ ДЛЯ mainwindow.cpp*/
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
static ChartView *ChartView1;
static Chart *Chart1;
static QLineSeries *Series1;
static QLineSeries *Series2;

static Chart *Chart2;
static QLineSeries *LineSeries1;
static QLineSeries *LineSeries2;
static ChartView *ChartView2;

static QValueAxis *axisX;
static QValueAxis *axisY;
static QCategoryAxis *axisZero;
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
using namespace QtCharts;
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
                    /*ФУНКЦИИ ПЕРВОЙ ВАЖНОСТИ*/
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
void MainWindow::_drawGraphic(QVector<double> dataArray, const double &minValueOfDataArray,
                              const double &maxValueOfDataArray)
{
    on_action_4_triggered();

    Chart1->removeSeries(Series1);
    Chart1->removeSeries(Series2);

    for(int i=0; i<dataArray.size()-1; i++)
    {
        Series1->append(i, dataArray[i]);
        //Series2
    }

    axisX->setRange(firstCount, secondCount);
    axisY->setRange(minValueOfDataArray, maxValueOfDataArray);
    axisZero->setRange(minValueOfDataArray, maxValueOfDataArray);
    Chart1->addSeries(Series1);
    Chart1->addSeries(Series2);
}

void MainWindow::on_action_4_triggered()
{
    Series1->clear();
    Series2->clear();
    LineSeries1->clear();
    LineSeries2->clear();
}

void MainWindow::showError(QString st)
{
    QPixmap MainIcon(":/resource/img/icons8-plus-48.png");
    QMessageBox msgBox(QMessageBox::NoIcon, tr("Ошибка"), st, QMessageBox::Ok);

    msgBox.setWindowIcon(MainIcon);
    msgBox.exec();
}

void MainWindow::on_DrawBtn_clicked()
//Начало расчета и вывода графиков
{
    ui->textEdit->clear();
    ui->textEdit_2->clear();

    MainWindow::on_action_4_triggered();

    bool ok;
    firstCount = ui->lineEdit_2->text().toInt(&ok);
    if (!ok) {
        showError("Ошибка ввода начального значения отсчета");
        return;
    }
    secondCount = ui->lineEdit->text().toInt(&ok);
    if (!ok) {
        showError("Ошибка ввода конечного значения отсчета");
        return;
    }

    if ((firstCount>=secondCount-3) || (firstCount<0)) {
        showError("Не верно введены значения отсчетов");
        return;
    }

    selectedLead = ui->comboBox->currentIndex()+1;

    QThread *Thread = new QThread;
    Calc *TCalc = new Calc();

    TCalc->fileName = fileName;
    TCalc->selectedLead = selectedLead;
    TCalc->firstCount = firstCount;
    TCalc->secondCount = secondCount;

    connect(TCalc, SIGNAL(drawGraphic(QVector<double>, const double &, const double &)),
            this, SLOT(_drawGraphic(QVector<double>, const double &, const double &)));
    connect(TCalc, SIGNAL(sendError(QString)), this, SLOT(showError(QString)));

    connect(Thread, SIGNAL(started() ), TCalc, SLOT(doCalc() ));

    connect(TCalc, SIGNAL(finished(QVector<double>) ), this, SLOT(newTaskLFHF(QVector<double>) ));
    connect(TCalc, SIGNAL(finished(QVector<double>) ), Thread, SLOT(quit() ));
    connect(TCalc, SIGNAL(finished(QVector<double>) ), TCalc, SLOT(deleteLater() ));
    connect(Thread, SIGNAL(finished() ), Thread, SLOT(deleteLater() ));

    TCalc->moveToThread(Thread);

    Thread->start();

}

void MainWindow::newTaskLFHF(QVector<double> Array)
{
    //размер массива кратен степени 2
    int n = 0;
    while (Array.size() > qPow(2 , n)){
        n++;
    }
    for (int i=0; Array.size()<qPow(2, n); i++){
        Array.push_back(0);
    }
    //______________________________________
    QThread *thread = new QThread;
    hflf *HFLF = new hflf(Array, Array.size());

    HFLF->moveToThread(thread);

    connect(thread, SIGNAL(started() ), HFLF, SLOT(doCalc() ));
    connect(HFLF, SIGNAL(sendArray(QVector<double>) ), this, SLOT(checkHFLF(QVector<double>) ));
    connect(HFLF, SIGNAL(finished() ), thread, SLOT(quit() ));
    connect(HFLF, SIGNAL(finished() ), HFLF, SLOT(deleteLater() ));
    connect(thread, SIGNAL(finished() ), thread, SLOT(deleteLater() ));

    thread->start();

}

void MainWindow::checkHFLF(QVector<double> a)
{
    ui->textEdit->insertPlainText(QString::number(a[0]));
}
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
                    /*ФУНКЦИИ ВТОРОЙ ВАЖНОСТИ*/
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Заголовок приложения
    MainWindow::setWindowTitle("Экстренное оказание кардиологической помощи");

    //Иконка кнопки FileOpen
    QPixmap pixFolder(":/resource/img/icons8-folder-48.png");
    int pixFolder_w = ui->FileBtn->width();
    int pixFolder_h = ui->FileBtn->height();
    ui->FileBtn->setIcon(pixFolder.scaled( pixFolder_w, pixFolder_h, Qt::KeepAspectRatio ));

    //Иконка кнопки Draw
    QPixmap pixDraw(":/resource/img/iconfinder_15_3049264.png");
    int pixDraw_w = ui->DrawBtn->width();
    int pixDraw_h = ui->DrawBtn->height();
    ui->DrawBtn->setIcon(pixDraw.scaled( pixDraw_w, pixDraw_h, Qt::KeepAspectRatio ));

    //Иконка приложения
    QIcon icon(":/resource/img/icons8-plus-48.png");
    MainWindow::setWindowIcon(icon);

    //Создаем Chart
    Chart1 = new Chart();
    Chart2 = new Chart();
    Series1 = new QLineSeries();
    Series2 = new QLineSeries();
    LineSeries1 = new QLineSeries();
    LineSeries2 = new QLineSeries();

    Series1->setColor( QColor(Qt::red) );
    Series2->setColor( QColor(Qt::green) );
    LineSeries1->setColor( QColor(Qt::darkBlue) );
    LineSeries2->setColor( QColor(Qt::green) );

    Chart1->addSeries(Series1);
    Chart1->addSeries(Series2);
    Chart1->legend()->hide();

    axisX = new QValueAxis;
    axisY = new QValueAxis;
    axisZero = new QCategoryAxis;

    axisX->setRange(0,100);
    axisX->setTickCount(11);
    axisX->setTickInterval(20);
    axisX->setLabelFormat("%i");

    axisY->setRange(0,100);
    axisY->setTickCount(11);
    axisY->setTickInterval(20);
    axisY->setLabelFormat("%i");

    axisZero->append("", 0);
    axisZero->setGridLineColor(QColor(Qt::darkBlue));

    Chart1->addAxis(axisX, Qt::AlignBottom);
    Chart1->addAxis(axisY, Qt::AlignLeft);
    Chart1->addAxis(axisZero, Qt::AlignRight);

    Series1->attachAxis(axisX);
    Series1->attachAxis(axisY);
    Series2->attachAxis(axisX);
    Series2->attachAxis(axisY);
    Series1->attachAxis(axisZero);
    Series2->attachAxis(axisZero);

    Chart2->addSeries(LineSeries1);
    Chart2->addSeries(LineSeries2);
    Chart2->legend()->hide();
    Chart2->createDefaultAxes();

    ChartView1 = new ChartView(Chart1);
    ChartView1->setRenderHint(QPainter::Antialiasing);

    QGridLayout *layout1 = new QGridLayout;
    layout1->addWidget(ChartView1);
    layout1->setMargin(0);
    ui->tab->setLayout(layout1);

    ChartView2 = new ChartView(Chart2);
    ChartView2->setRenderHint(QPainter::Antialiasing);

    QGridLayout *layout2 = new QGridLayout;
    layout2->addWidget(ChartView2);
    layout2->setMargin(0);
    ui->tab_2->setLayout(layout2);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_FileBtn_clicked()
//Диалог с открытием файла
{
    fileName = QFileDialog::getOpenFileName(this, tr("Открытие файла с данными"),
                                            tr(""), tr("TextFile (*.txt) ") );
}

void MainWindow::on_action_2_triggered()
//Диалог с открытием файла
{
    MainWindow::on_FileBtn_clicked();
}

void MainWindow::on_action_3_triggered()
//Выход
{
    MainWindow::close();
}

void MainWindow::on_action_5_triggered()
{
    Chart1->zoomReset();
    Chart2->zoomReset();
    ChartView1->repaint();
    ChartView2->repaint();
}

void MainWindow::on_action_triggered()
{
    on_DrawBtn_clicked();
}
