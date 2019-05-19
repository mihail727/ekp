#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QPixmap"

#include <Methods/hflf.h>
#include <Methods/calc.h>
#include <Methods/cqrs.h>

using namespace QtCharts;

/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
                    /*ФУНКЦИИ ПЕРВОЙ ВАЖНОСТИ*/
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
void MainWindow::_drawGraphic(QVector<double> dataArray)
{
//Вывод графика
    ui->chart1->clearGraphs();

    ui->chart1->addGraph(); //graph(0)
    ui->chart1->addGraph(); //graph(1)

    ui->chart1->graph(0)->setPen(QPen(Qt::red));
    ui->chart1->graph(1)->setPen(QPen(Qt::blue));

    //формирование графика ЭКГ
    QVector<double> x_value, y_value, x_zero, y_zero;
    for(int i=0; i<dataArray.size(); i++) {
        x_value.push_back(i);
        y_value.push_back(dataArray[i]);
        x_zero.push_back(i);
        y_zero.push_back(0);
    }

    ui->chart1->graph(0)->setData(x_value, y_value);
    ui->chart1->graph(0)->rescaleAxes();
    ui->chart1->graph(1)->setData(x_zero, y_zero);

    QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
    ui->chart1->xAxis->setTicker(fixedTicker);
    ui->chart1->yAxis->setTicker(fixedTicker);
    fixedTicker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples);

    ui->chart1->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->chart1->replot();
}

void MainWindow::showError(QString st)
//--------------------------------------------------------------------------------------------
//Вывод сообщения об ошибке + вывод сообщения
{
    QPixmap MainIcon(":/resource/img/icons8-plus-48.png");
    QMessageBox msgBox(QMessageBox::NoIcon, tr("Ошибка"), st, QMessageBox::Ok);

    msgBox.setWindowIcon(MainIcon);
    msgBox.exec();
//--------------------------------------------------------------------------------------------
}

void MainWindow::on_action_triggered()
//--------------------------------------------------------------------------------------------
//Начало расчета и вывода графиков
{
    ui->textEdit->clear();
    ui->textEdit_2->clear();

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
//--------------------------------------------------------------------------------------------
//Обработка файла + создание массива с исходными данными
    QThread *Thread = new QThread;
    Calc *TCalc = new Calc();

    connect(TCalc, SIGNAL(drawGraphic(QVector<double>)), this, SLOT(_drawGraphic(QVector<double>)));
    connect(TCalc, SIGNAL(sendError(QString)), this, SLOT(showError(QString)));

    connect(TCalc, SIGNAL(finished(QVector<double>) ), Thread, SLOT(quit() ));
    connect(TCalc, SIGNAL(finished(QVector<double>) ), TCalc, SLOT(deleteLater() ));
    connect(Thread, SIGNAL(finished() ), Thread, SLOT(deleteLater() ));

    connect(Thread, &QThread::started, TCalc, [=] {
        TCalc->doCalc(fileName, selectedLead, firstCount, secondCount);
    });
    //по заврешению работы TCalc запускатеся newTaskLFHF
    connect(TCalc, SIGNAL(finished(QVector<double>) ), this, SLOT(newTaskLFHF(QVector<double>) ));
    //по завершению работы TCalc запускается newTaskQRS
    connect(TCalc, SIGNAL(finished(QVector<double>) ), this, SLOT(newTaskQRS(QVector<double>) ));

    TCalc->moveToThread(Thread);

    Thread->start();
//--------------------------------------------------------------------------------------------
}

void MainWindow::newTaskQRS(QVector<double> Array)
//Выполнение метода Пана-Томпкинса --- вывод графика QRS
{
    QThread *thread = new QThread;
    cQRS *qrs = new cQRS();

    qrs->moveToThread(thread);

    connect(thread, &QThread::started, qrs, [=] {
        qrs->doCalc(Array, firstCount, secondCount);
    });

    connect(qrs, SIGNAL(finished() ), thread, SLOT(quit() ));
    connect(qrs, SIGNAL(finished() ), qrs, SLOT(deleteLater() ));
    connect(thread, SIGNAL(finished() ), thread, SLOT(deleteLater() ));
    connect(qrs, SIGNAL(sendQRSValues(QVector<double> , QVector<double>) ),
            this, SLOT(drawQRS(QVector<double>, QVector<double> )));

    thread->start();
    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::drawQRS(QVector<double> ArrayData, QVector<double> x_picks)
//Вывод графика QRS
{
    ui->chart2->addGraph(); //graph0

    ui->chart2->graph(0)->setPen(QPen(Qt::black));

    //формирование графика QRS
    QVector<double> x_value, y_value, y_picks;
    int j=0;
    for(int i=firstCount; i<ArrayData.size(); i++) {
        x_value.push_back(i);
        y_value.push_back(ArrayData[i]);
    }

    ui->chart2->graph(0)->setData(x_value, y_value);
    ui->chart2->graph(0)->rescaleAxes();

    QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
    ui->chart2->xAxis->setTicker(fixedTicker);
    ui->chart2->yAxis->setTicker(fixedTicker);
    fixedTicker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples);

    ui->chart2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    //вывод макс точек на qrs график
    ui->chart2->addGraph(); //graph2
    ui->chart2->graph(1)->setPen(QPen(Qt::red));

    for(int i=0; i<ArrayData.size(); i++) {
        if( (j<x_picks.size()) && (int(x_picks[j]) == i) ) {
            y_picks.push_back(ArrayData[i]);
            j++;
        }
    }

    for(int i=0; i<x_picks.size(); i++)
        qDebug() << x_picks[i] << y_picks[i];

    ui->chart2->graph(1)->setData(x_picks, y_picks);
    ui->chart2->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QColor(Qt::blue), QColor(Qt::red), 7));

    ui->chart2->replot();
}

void MainWindow::newTaskLFHF(QVector<double> Array)
{
//--------------------------------------------------------------------------------------------
//Размер массива становится кратен степени 2
    int n = 0;
    while (Array.size() > qPow(2 , n)){
        n++;
    }
    for (int i=0; Array.size()<qPow(2, n); i++){
        Array.push_back(0);
    }
    //______________________________________
    QThread *thread = new QThread;
    hflf *HFLF = new hflf();

    HFLF->moveToThread(thread);

    connect(thread, &QThread::started, HFLF, [=] {
        HFLF->doCalc(Array, Array.size());
    });

    connect(HFLF, SIGNAL(sendArray(QVector<double>) ), this, SLOT(checkHFLF(QVector<double>) ));
    connect(HFLF, SIGNAL(finished() ), thread, SLOT(quit() ));
    connect(HFLF, SIGNAL(finished() ), HFLF, SLOT(deleteLater() ));
    connect(thread, SIGNAL(finished() ), thread, SLOT(deleteLater() ));

    thread->start();
//--------------------------------------------------------------------------------------------
}

void MainWindow::checkHFLF(QVector<double> a)
//для проверки HFLF массива --потом убрать!
{
    ui->textEdit->insertPlainText(QString::number(a[0]));
}
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
                    /*ФУНКЦИИ ВТОРОЙ ВАЖНОСТИ*/
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->toolBar_2 && acceptDrag)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->button() == Qt::LeftButton)
            {
                dragPosition = mouse_event->globalPos() - frameGeometry().topLeft();
                return false;
            }
        }
        else if (event->type() == QEvent::MouseMove)
        {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->buttons() & Qt::LeftButton)
            {
                move(mouse_event->globalPos() - dragPosition);
                return false;
            }
        }
    }
    return false;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent, Qt::FramelessWindowHint),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->toolBar_2->installEventFilter(this);

    ui->toolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    ui->toolBar_2->setContextMenuPolicy(Qt::PreventContextMenu);
    ui->toolBar_3->setContextMenuPolicy(Qt::PreventContextMenu);
    ui->toolBar_4->setContextMenuPolicy(Qt::PreventContextMenu);

    //Иконка приложения
    QLabel *WinIconTitle = new QLabel();
    QIcon WinIcon(":/resource/img/icons8-plus-48.png");
    QPixmap pixmapWinIcon = WinIcon.pixmap(QSize(25, 25));
    WinIconTitle->setPixmap(pixmapWinIcon);
    ui->toolBar_2->addWidget(WinIconTitle);

    //title приложения
    QLabel *WinTitle = new QLabel("Экстренное оказание кардиологической помощи");
    ui->toolBar_2->addWidget(WinTitle);


    //dock для toolbar *alignRight*
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->toolBar_2->addWidget(spacer);

    //свернуть
    QIcon HideIcon(":/resource/img/icons8-subtract-filled-40.png");
    ui->toolBar_2->addAction(HideIcon, "Свернуть приложение", this, SLOT(showMinimized()));

    //расширить окно на фул экран
    QIcon MaxIcon(":/resource/img/icons8-rectangular-96.png");
    QAction *action_max = new QAction();
    action_max->setIcon(MaxIcon);
    action_max->setText("Развернуть окно на весь экран");
    auto showFullScreen_ = [this]
    {
        oldGeometry = this->geometry();
        auto screenGeometry = QGuiApplication::screens().first()->availableGeometry();
        this->setGeometry(screenGeometry);
        acceptDrag = false;
    };
    connect(action_max, &QAction::triggered, showFullScreen_);
    ui->toolBar_2->addAction(action_max);
    action_max->setVisible(true);

    //вернуть размер окна в начальное
    QIcon MinIcon(":/resource/img/icons8-ungroup-objects-96.png");
    QAction *action_min = new QAction();
    action_min->setIcon(MinIcon);
    action_min->setText("Вернуть окно в начальный размер");
    auto showNormal_ = [this]
    {
        this->setGeometry(oldGeometry);
        acceptDrag = true;
    };
    connect(action_min, &QAction::triggered, showNormal_);
    ui->toolBar_2->addAction(action_min);
    action_min->setVisible(false);

    auto set_Visible_For_action_min = [action_max, action_min]() {
        action_min->setVisible(true);
        action_max->setVisible(false);
    };
    connect(action_max, &QAction::triggered, set_Visible_For_action_min);

    auto set_Visible_For_action_max = [action_max, action_min]() {
        action_max->setVisible(true);
        action_min->setVisible(false);
    };
    connect(action_min, &QAction::triggered, set_Visible_For_action_max);

    //закрыть
    QIcon CloseIcon(":/resource/img/icons8-delete-512.png");
    ui->toolBar_2->addAction(CloseIcon, "Закрыть приложение", qApp, SLOT(quit()));

    //Иконка приложения
    QIcon icon(":/resource/img/icons8-plus-48.png");
    MainWindow::setWindowIcon(icon);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_2_triggered()
//Диалог с открытием файла
{
    fileName = QFileDialog::getOpenFileName(this, tr("Открытие файла с данными"),
                                            tr(""), tr("TextFile (*.txt) ") );
}
