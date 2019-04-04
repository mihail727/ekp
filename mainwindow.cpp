#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QPixmap"
#include "QtCharts/QtCharts"
#include <calc.h>

QString fileName;  //путь к файлу
int selectedLead; //Выбранное отведение
int firstCount, secondCount; //начало и конец отсчетов <<--->> количество отсчетов
QVector<double> dataArray;

static QChart *Chart1 ;
static QLineSeries *Series1;
static QLineSeries *Series2;
static QLineSeries *Series3;

static QChart *Chart2;
static QLineSeries *LineSeries1;
static QLineSeries *LineSeries2;
static QLineSeries *LineSeries3;
static QChartView *ChartView1;
static QChartView *ChartView2;

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
    Chart1 = new QChart();
    Chart2 = new QChart();
    Series1 = new QLineSeries();
    Series2 = new QLineSeries();
    Series3 = new QLineSeries();
    LineSeries1 = new QLineSeries();
    LineSeries2 = new QLineSeries();
    LineSeries3 = new QLineSeries();

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

    QValueAxis *axisX = new QValueAxis;
    QValueAxis *axisY = new QValueAxis;

    axisX->setRange(0,100);
    axisX->setTickCount(11);
    axisX->setLabelFormat("%i");

    axisY->setRange(0,100);
    axisY->setTickCount(11);
    axisY->setLabelFormat("%i");

    Chart1->addAxis(axisX, Qt::AlignBottom);
    Chart1->addAxis(axisY, Qt::AlignLeft);

    Series1->attachAxis(axisX);
    Series1->attachAxis(axisY);
    Series3->attachAxis(axisX);
    Series3->attachAxis(axisY);
    Series2->attachAxis(axisX);
    Series2->attachAxis(axisY);

    Series1->setUseOpenGL(true);
    Series2->setUseOpenGL(true);
    Series3->setUseOpenGL(true);

    Chart2->addSeries(LineSeries1);
    Chart2->addSeries(LineSeries2);
    Chart2->addSeries(LineSeries3);
    Chart2->legend()->hide();
    Chart2->createDefaultAxes();

    ChartView1 = new QChartView(Chart1);
    ChartView1->setRenderHint(QPainter::Antialiasing);
    QGridLayout *layout1 = new QGridLayout;
    layout1->addWidget(ChartView1);
    layout1->setMargin(0);
    ui->tab->setLayout(layout1);

    ChartView2 = new QChartView(Chart2);
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

void MainWindow::showInfo(QString st)
{
    ui->textEdit_2->insertPlainText(st);
}

void MainWindow::showError(QString st)
//Update progress bar
{
    QPixmap MainIcon(":/resource/img/icons8-plus-48.png");
    QMessageBox msgBox(QMessageBox::NoIcon, tr("Ошибка"), st, QMessageBox::Ok);

    msgBox.setWindowIcon(MainIcon);
    msgBox.exec();
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

void MainWindow::on_DrawBtn_clicked()
//Начало расчета и вывода графиков
{    
    ui->textEdit->clear();
    ui->textEdit_2->clear();

    MainWindow::on_action_4_triggered();

    firstCount = ui->lineEdit_2->text().toInt();
    secondCount = ui->lineEdit->text().toInt();

    QThread *Thread = new QThread;
    Calc *TCalc = new Calc;

    selectedLead = ui->comboBox->currentIndex()+1;

    TCalc->moveToThread(Thread);

    connect(TCalc, SIGNAL(drawGraphic() ), this, SLOT(_drawGraphic() ));

    connect(TCalc, SIGNAL(sendError(QString)), this, SLOT(showError(QString)));

    connect(TCalc, SIGNAL(send(QString)), this, SLOT(showInfo(QString)));

    connect(Thread, SIGNAL(started()), TCalc, SLOT(doCalc()));

    Thread->start();

}
void MainWindow::_drawGraphic()
{
    for(int i=0; i<dataArray.size()-1; i++)
    {
        Series1->append(i, dataArray[i]);
        Series2->append(i, 0);
    }
    Chart1->addSeries(Series1);
    Chart1->addSeries(Series2);
    Chart1->addSeries(Series3);
};

void MainWindow::on_action_4_triggered()
{
    Series1->clear();
    Series2->clear();
    Series3->clear();
    Chart1->removeSeries(Series1);
    Chart1->removeSeries(Series2);
    Chart1->removeSeries(Series3);
    LineSeries1->clear();
    LineSeries2->clear();
    LineSeries3->clear();
    Chart2->removeSeries(LineSeries1);
    Chart2->removeSeries(LineSeries2);
    Chart2->removeSeries(LineSeries3);
}
