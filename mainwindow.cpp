#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QPixmap"
#include "QtCharts/QtCharts"

using namespace QtCharts;

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
    QChart *Chart1 = new QChart();
    QChart *Chart2 = new QChart();
    QLineSeries *Series1 = new QLineSeries();
    QLineSeries *Series2 = new QLineSeries();
    QLineSeries *Series3 = new QLineSeries();
    QLineSeries *LineSeries1 = new QLineSeries();
    QLineSeries *LineSeries2 = new QLineSeries();
    QLineSeries *LineSeries3 = new QLineSeries();

//    for (int i=0; i<10; i++) //Для заполнения
//    {
//        Series1->append(i,i+10);
//        Series2->append(i,15);
//    }

    Series1->setColor( QColor(Qt::red) );
    Series2->setColor( QColor(Qt::darkBlue) );
    Series3->setColor( QColor(Qt::green) );
    LineSeries1->setColor( QColor(Qt::darkBlue) );
    LineSeries2->setColor( QColor(Qt::darkBlue) );
    LineSeries3->setColor( QColor(Qt::green) );

    Chart1->addSeries(Series1);
    Chart1->addSeries(Series2);
    Chart1->addSeries(Series3);
    Chart1->legend()->hide();
    Chart1->createDefaultAxes();

    Chart2->addSeries(LineSeries1);
    Chart2->addSeries(LineSeries2);
    Chart2->addSeries(LineSeries3);
    Chart2->legend()->hide();
    Chart2->createDefaultAxes();

    QChartView *ChartView1 = new QChartView(Chart1);
    ChartView1->setRenderHint(QPainter::Antialiasing);
    QChartView *ChartView2 = new QChartView(Chart2);
    ChartView2->setRenderHint(QPainter::Antialiasing);

    QGridLayout *layout1 = new QGridLayout;
    layout1->addWidget(ChartView1);
    layout1->setMargin(0);
    ui->tab->setLayout(layout1);
    QGridLayout *layout2 = new QGridLayout;
    layout2->addWidget(ChartView2);
    layout2->setMargin(0);
    ui->tab_2->setLayout(layout2);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update(int i)
//Update progress bar
{
    ui->lineEdit->setText("Hello");
}

void MainWindow::on_FileBtn_clicked()
//Диалог с открытием файла
{
    fileName = QFileDialog::getOpenFileName(this,tr("Open TextFile"),tr(""), tr("TextFile (*.txt) ") );
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

void MainWindow::on_DrawBtn_clicked()
//Начало расчета и вывода графиков
{
    QThread *Thread = new QThread;
    Calc *TCalc = new Calc;

    TCalc->moveToThread(Thread);

    connect(TCalc, SIGNAL(send(int)),this, SLOT(update()));
    connect(Thread, SIGNAL(started()),TCalc,SLOT(doCalc()));
}
