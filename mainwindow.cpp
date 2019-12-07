#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QPixmap"
#include <QtCore/qmath.h>

#include <Methods/hflf.h>
#include <Methods/calc.h>
#include <Methods/cqrs.h>
#include <Methods/emd.h>
#include <Methods/fazagrov.h>

static int rand1 = 1;

/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
                    /*ФУНКЦИИ ПЕРВОЙ ВАЖНОСТИ*/
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

void MainWindow::newTaskEMD(QVector<double> Array)
{
    QThread *thread = new QThread;
    cEMD *emd = new cEMD();

    emd->moveToThread(thread);

    connect(thread, &QThread::started, emd, [=] {
        emd->doCalc(Array, firstCount, secondCount);
    });

    connect(emd, SIGNAL(finished()), thread, SLOT(quit()));
    connect(emd, SIGNAL(finished() ), emd, SLOT(deleteLater() ));
    connect(thread, SIGNAL(finished() ), thread, SLOT(deleteLater() ));

    //по завершению cEMD выодится график chart2
    connect(emd, SIGNAL(drawEMD(QVector<double>, QVector<double>)),
            this, SLOT(_drawEMD(QVector<double>, QVector<double>)));

    thread->start();
}

void MainWindow::_drawFazagrov(QVector<double> arr)
{
    ui->chart3->addGraph(); //graph0
    ui->chart3->graph(0)->setPen(QPen(Qt::blue));
    QVector<double> Zero, Zero_x;
    for(int i=0; i<arr.size(); i++) {
        Zero.push_back(0);
        Zero_x.push_back(i);
    }

    ui->chart3->addGraph(); //graph1
    ui->chart3->graph(1)->setPen(QPen(Qt::red));
    QVector<double> arr_x;
    for(int i=0; i<arr.size(); i++)
        arr_x.push_back(i);

    ui->chart3->graph(0)->setData(Zero_x, Zero);
    ui->chart3->graph(0)->rescaleAxes();
    ui->chart3->graph(1)->setData(arr_x, arr);
    ui->chart3->graph(1)->rescaleAxes();

    ui->chart3->yAxis->setRange( ui->chart3->yAxis->range().lower-100, ui->chart3->yAxis->range().upper+300 );

    ui->chart3->replot();
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
    //по заверщению cqrs выводится график drawQRS
    connect(qrs, SIGNAL(sendValues_for_drawGraphic(QVector<double>, QVector<double>,
                                                   QVector<double>, QVector<double>,
                                                   QVector<double>, QVector<double>,
                                                   QVector<double>, QVector<double>)  ),
            this, SLOT(drawQRS(QVector<double>, QVector<double>,
                               QVector<double>, QVector<double>,
                               QVector<double>, QVector<double>,
                               QVector<double>, QVector<double>) ));

    //по завершению начинается расчет Пульса, SDANN, varQT
    connect(qrs, SIGNAL(sendValues_for_calculate(QVector<double>, QVector<double>,
                                                 QVector<double>) ),
            this, SLOT(CalculateSomeProc(QVector<double>, QVector<double>,
                                         QVector<double> )));
    //по завершению работы newTaskQRS
    connect(qrs, SIGNAL(finished()), this, SLOT(Calc_Diag()));

    thread->start();
}

void MainWindow::CalculateSomeProc(QVector<double> RValues, QVector<double> QValues, QVector<double> SValues)
//Расчет дополнительных параметров (Пульс, SDANN, varQT ...)
{
    if(RValues.size() > 2)
    {
        //Пульс
        double difValue = 0;
        for(int i=0; i<RValues.size()-1; i++)
            difValue = difValue + abs(RValues[i] - RValues[i+1]);
        difValue = difValue / (RValues.size()-1);
        int pulse;
        pulse = int( (1/difValue*1000)*60 );
        difValue = int(difValue);
        //++++++++++++++++++++++++++++++++++++

        //SDANN
        int summ = 0;
        for(int i=0; i< RValues.size()-1; i++)
            summ = summ + int( qPow( abs(RValues[i] - RValues[i+1]) - difValue, 2) );
        summ = int( ( 1.0/(RValues.size()-1) ) * qSqrt(summ) );
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++

        //varQt
        double max = RValues[0];
        double min = max;
        for(int i=0; i<RValues.size(); i++)
        {
            if(max < RValues[i])
                max = RValues[i];
            if(min > RValues[i])
                min = RValues[i];
        }
        double varQt;
        varQt = qCeil( 100/(max/min) );
        //+++++++++++++++++++++++++++++

        //t(RS)
        int difRS = 0;
        for(int i=0; i<RValues.size(); i++)
        {
            difRS = difRS + int(SValues[i] - RValues[i]);
        }
        difRS = difRS / RValues.size();
        //+++++++++++++++++++++++++++++

        //t(QR)
        int difQR = 0;
        for(int i=0; i<RValues.size(); i++)
        {
            difQR = difQR + int(RValues[i] - QValues[i]);
        }
        difQR = difQR / RValues.size();
        //+++++++++++++++++++++++++++++

        //t(QRS)
        int difQRS = 0;
        for(int i=0; i<RValues.size(); i++)
        {
            difQRS = difQRS + int(SValues[i] - QValues[i]);
        }
        difQRS = difQRS / RValues.size();
        //

        //Вывод данных
        ui->lineEdit_3->setText(QString::number(summ)); //SDANN
        ui->lineEdit_4->setText(QString::number(varQt)); //varQt
        ui->lineEdit_5->setText(QString::number(difQR)); //t(QR)
        ui->lineEdit_6->setText(QString::number(difRS)); //t(RS)
        ui->lineEdit_7->setText(QString::number(difValue)); //R, mc
        ui->lineEdit_10->setText(QString::number(difQRS)); //t(QRS)

        chartTitle->setText("Средняя длительность кардиоцикла: " + QString::number(difValue) + " (мс); "
                            + "Пульс: " + QString::number(pulse));
        chartTitle->setVisible(true);

        ui->chart1->replot();
        //
    }
    else {
        ui->lineEdit_3->clear(); //SDANN
        ui->lineEdit_4->clear(); //varQt
        ui->lineEdit_5->clear(); //t(QR)
        ui->lineEdit_6->clear(); //t(RS)
        ui->lineEdit_7->clear(); //R, mc
        ui->lineEdit_10->clear(); //t(QRS)
        ui->textEdit_2->clear(); //Диагноз

        chartTitle->setText("Средняя длительность кардиоцикла: -- ; Пульс: --");

        ui->chart1->replot();
    }
}

void MainWindow::_drawEMD(QVector<double> ArrayMain, QVector<double> ArrayEMD)
//Вывод графика EMD
{
    int j = 0;
    ui->chart2->addGraph(); //graph0
    ui->chart2->graph(0)->setPen(QPen(Qt::green));
    QVector<double> ArrayEMD_y;
    for(int i=0; i<ArrayMain.size(); i++){
        if( (j<ArrayEMD.size()) && (int(ArrayEMD[j]) == i) ) {
            ArrayEMD_y.push_back(ArrayMain[i]);
            j++;
        }
    }

    ui->chart2->graph(0)->setData(ArrayEMD, ArrayEMD_y);

    ui->chart2->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QColor(Qt::darkGreen), QColor(Qt::green), 7));

    ui->chart2->graph(0)->setLineStyle(QCPGraph::lsNone);

    ui->chart2->yAxis->setRange( ui->chart2->yAxis->range().lower-100, ui->chart2->yAxis->range().upper+300 );

    ui->chart2->replot();
}

void MainWindow::drawQRS(QVector<double> RValues, QVector<double> ArrMain,
                         QVector<double> QValues, QVector<double> SValues,
                         QVector<double> PValues, QVector<double> TValues,
                         QVector<double> NewVal1, QVector<double> NewVal2)
//Вывод графика QRS
{
    int j = 0;
    //вывод макс точек на график
    //формирование R пиков
    ui->chart1->addGraph(); //graph2
    ui->chart1->graph(2)->setPen(QPen(Qt::green));
    QVector<double> RValues_y;
    for(int i=0; i<ArrMain.size(); i++) {
        if( (j<RValues.size()) && (int(RValues[j]) == i) ) {
            RValues_y.push_back(ArrMain[i]);
            j++;
        }
    }

    //формирование Q точек
    ui->chart1->addGraph(); //graph3
    QVector<double> QValues_y; j=0;
    for(int i=0; i<ArrMain.size(); i++) {
        if( (j<QValues.size()) && (int(QValues[j]) == i) ) {
            QValues_y.push_back(ArrMain[i]);
            j++;
        }
    }

    //формирование S точек
    ui->chart1->addGraph(); //graph4
    QVector<double> SValues_y; j=0;
    for(int i=0; i<ArrMain.size(); i++) {
        if( (j<SValues.size()) && (int(SValues[j]) == i) ) {
            SValues_y.push_back(ArrMain[i]);
            j++;
        }
    }

    //формирование P точек
    ui->chart1->addGraph(); //graph5
    QVector<double> PValues_y; j=0;
    for(int i=0; i<ArrMain.size(); i++) {
        if( (j<PValues.size()) && (int(PValues[j]) == i) ) {
            PValues_y.push_back(ArrMain[i]);
            j++;
        }
    }

    //формирование T точек
    ui->chart1->addGraph(); //graph6
    QVector<double> TValues_y; j=0;
    for(int i=0; i<ArrMain.size(); i++) {
        if( (j<TValues.size()) && (int(TValues[j]) == i) ) {
            TValues_y.push_back(ArrMain[i]);
            j++;
        }
    }

    //формирование NewVal1 точек
    ui->chart1->addGraph(); //graph6
    QVector<double> NewVal1_y; j=0;
    for(int i=0; i<ArrMain.size(); i++) {
        if( (j<NewVal1.size()) && (int(NewVal1[j]) == i) ) {
            NewVal1_y.push_back(ArrMain[i]);
            j++;
        }
    }

    //формирование NewVal2 точек
    ui->chart1->addGraph(); //graph6
    QVector<double> NewVal2_y; j=0;
    for(int i=0; i<ArrMain.size(); i++) {
        if( (j<NewVal2.size()) && (int(NewVal2[j]) == i) ) {
            NewVal2_y.push_back(ArrMain[i]);
            j++;
        }
    }

    ui->chart1->graph(2)->setData(RValues, RValues_y);
    ui->chart1->graph(3)->setData(QValues, QValues_y);
    ui->chart1->graph(4)->setData(SValues, SValues_y);
    ui->chart1->graph(5)->setData(PValues, PValues_y);
    ui->chart1->graph(6)->setData(TValues, TValues_y);
    ui->chart1->graph(7)->setData(NewVal1, NewVal1_y);
    ui->chart1->graph(8)->setData(NewVal2, NewVal2_y);

    ui->chart1->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QColor(Qt::darkGreen), QColor(Qt::green), 7));
    ui->chart1->graph(3)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QColor(Qt::darkGreen), QColor(Qt::red), 7));
    ui->chart1->graph(4)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QColor(Qt::darkGreen), QColor(Qt::yellow), 7));
    ui->chart1->graph(5)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QColor(Qt::darkGreen), QColor(Qt::magenta), 7));
    ui->chart1->graph(6)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QColor(Qt::darkGreen), QColor(Qt::blue), 7));
    ui->chart1->graph(7)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QColor(Qt::darkGreen), QColor(171, 0, 255), 7));
    ui->chart1->graph(8)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QColor(Qt::darkGreen), QColor(255, 255, 255), 7));

    ui->chart1->graph(3)->setLineStyle(QCPGraph::lsNone);
    ui->chart1->graph(4)->setLineStyle(QCPGraph::lsNone);
    ui->chart1->graph(5)->setLineStyle(QCPGraph::lsNone);
    ui->chart1->graph(6)->setLineStyle(QCPGraph::lsNone);
    ui->chart1->graph(7)->setLineStyle(QCPGraph::lsNone);
    ui->chart1->graph(8)->setLineStyle(QCPGraph::lsNone);

    ui->chart1->yAxis->setRange( ui->chart1->yAxis->range().lower-100, ui->chart1->yAxis->range().upper+300 );

    ui->chart1->replot();
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
    //ui->textEdit->insertPlainText(QString::number(a[0]));
}

/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
                    /*ФУНКЦИИ ВТОРОЙ ВАЖНОСТИ*/
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    //События для перетаскивания окна
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
    ui->toolBar_2->setMouseTracking(true);

    ui->toolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    ui->toolBar_2->setContextMenuPolicy(Qt::PreventContextMenu);
    ui->toolBar_3->setContextMenuPolicy(Qt::PreventContextMenu);
    ui->toolBar_4->setContextMenuPolicy(Qt::PreventContextMenu);

    //Заголовк графика (пульс + сред.откл)
    chartTitle = new QCPItemText(ui->chart1);
    chartTitle->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
    chartTitle->position->setType(QCPItemPosition::ptAxisRectRatio);
    chartTitle->position->setCoords(0.5, 0);
    chartTitle->setFont(QFont("Tahoma", 11));
    chartTitle->setVisible(false);

    //Иконка приложения
    QLabel *WinIconTitle = new QLabel();
    QIcon WinIcon(":/resource/img/icons8-plus-48.png");
    QPixmap pixmapWinIcon = WinIcon.pixmap(QSize(20, 20));
    WinIconTitle->setPixmap(pixmapWinIcon);
    ui->toolBar_2->addWidget(WinIconTitle);

    //title приложения
    QLabel *WinTitle = new QLabel("Экстренное оказание кардиологической помощи");
    WinTitle->setStyleSheet("QLabel{ font-size: 12px;"
                                    "font: Bold;"
                                    "margin: 1px; }");
    ui->toolBar_2->addWidget(WinTitle);

    //QSizeGrip растягивание окна за угл
    QSizeGrip* SizeGrip = new QSizeGrip(this);
    ui->toolBar_3->addWidget(SizeGrip);
    QWidget* spacer_ = new QWidget();
    spacer_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->toolBar_3->addWidget(spacer_);
    QSizeGrip* SizeGrip_ = new QSizeGrip(this);
    ui->toolBar_3->addWidget(SizeGrip_);

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

void MainWindow::_drawGraphic(QVector<double> dataArray)
{
//Вывод графика
    ui->chart1->clearGraphs();
    ui->chart2->clearGraphs();
    ui->chart3->clearGraphs();

    ui->chart1->addGraph(); //graph(0)
    ui->chart1->addGraph(); //graph(1)

    ui->chart1->graph(0)->setPen(QPen(Qt::red));
    ui->chart1->graph(1)->setPen(QPen(Qt::blue));

    //формирование графика ЭКГ
    QVector<double> x_value, y_value, x_zero, y_zero;
    int buff = firstCount;
    for(int i=0; i<dataArray.size(); i++) {
        x_value.push_back(buff);
        y_value.push_back(dataArray[i]);
        x_zero.push_back(buff);
        y_zero.push_back(0);
        buff++;
    }

    ui->chart1->graph(0)->setData(x_value, y_value);
    ui->chart1->graph(0)->rescaleAxes();
    ui->chart1->graph(1)->setData(x_zero, y_zero);

    QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
    ui->chart1->xAxis->setTicker(fixedTicker);
    ui->chart1->yAxis->setTicker(fixedTicker);

    ui->chart2->xAxis->setTicker(fixedTicker);
    ui->chart2->yAxis->setTicker(fixedTicker);

    ui->chart3->xAxis->setTicker(fixedTicker);
    ui->chart3->yAxis->setTicker(fixedTicker);

    fixedTicker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples);
    fixedTicker->setTickCount(16);

    ui->chart1->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->chart2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->chart3->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    ui->chart1->axisRect()->setRangeZoom(Qt::Horizontal);
    ui->chart2->axisRect()->setRangeZoom(Qt::Horizontal);
    ui->chart3->axisRect()->setRangeZoom(Qt::Horizontal);

    ui->chart1->replot();
}

void MainWindow::on_pushButton_clicked()
//Начало расчета диагноза
{
    if(fileName == ""){
        showError("Откройте файл...");
        return;
    }

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

    if ((firstCount>=secondCount-3) || (firstCount<0) || (secondCount - firstCount < 3000)) {
        showError("Не верно введены значения отсчетов");
        return;
    }

    selectedLead = ui->comboBox->currentIndex()+1;
//--------------------------------------------------------------------------------------------
//Обработка файла + создание массива с исходными данными
    Method = ekp;

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
    //по завершению работы TCalc запускается newTaskEMD
    connect(TCalc, SIGNAL(finished(QVector<double>) ), this, SLOT(newTaskEMD(QVector<double>) ));

    TCalc->moveToThread(Thread);

    Thread->start();
}

void MainWindow::on_pushButton_3_clicked()
//Fazagrov
{
    if(fileName == ""){
        showError("Откройте файл...");
        return;
    }

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

    if ((firstCount>=secondCount-3) || (firstCount<0) || (secondCount - firstCount < 3000)) {
        showError("Не верно введены значения отсчетов");
        return;
    }

    selectedLead = ui->comboBox->currentIndex()+1;
//--------------------------------------------------------------------------------------------
//Обработка файла + создание массива с исходными данными
    Method = fazagrov;

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

    connect(TCalc, SIGNAL(finished(QVector<double>) ), this, SLOT(newTaskFazagrov(QVector<double>) ));

    TCalc->moveToThread(Thread);

    Thread->start();
}

void MainWindow::newTaskFazagrov(QVector<double> arr)
{
    QThread *thread = new QThread;
    Fazagrov *faz = new Fazagrov();

    faz->moveToThread(thread);

    connect(thread, &QThread::started, faz, [=] {
        faz->doCalc(arr, firstCount, secondCount);
    });

    connect(faz, SIGNAL(finished()), thread, SLOT(quit()));
    connect(faz, SIGNAL(finished() ), faz, SLOT(deleteLater() ));
    connect(thread, SIGNAL(finished() ), thread, SLOT(deleteLater() ));

    //по завершению Fazagrov выводится график chart3
    connect(faz, SIGNAL(draw_graphic(QVector<double>)),
            this, SLOT(_drawFazagrov(QVector<double>)));

    thread->start();
}

void MainWindow::Calc_Diag()
{
    int varQT = ui->lineEdit_4->text().toInt();
    int SDANN = ui->lineEdit_3->text().toInt();
    int valR = ui->lineEdit_7->text().toInt();
    int tRS = ui->lineEdit_6->text().toInt();
    int tQR = ui->lineEdit_5->text().toInt();
    int QRS = ui->lineEdit_10->text().toInt();
    int valLFHF = ui->lineEdit_9->text().toInt();
    int valST = ui->lineEdit_8->text().toInt();
    double k;
    if(ui->radioButton->isChecked()) k = 0.37;
    else if(ui->radioButton_3->isChecked()) k = 0.39;
    else k = 0.38;

    //+++++КСР КДР+++++
    enum cGysa{right, left, none}; cGysa Gysa = none;
    if(ui->checkBox->isChecked()) //Блокада Гиса
    {
        if(ui->radioButton_5->isChecked()) Gysa = right;
        else Gysa = left;
    }

    double KCR, KDR;
    KDR = (44.5 - 100*tRS) * (tQR+tRS) - 11*tRS;

    if(Gysa == none)
    {
        KCR = (44.5 - 100*tRS) * (tQR+tRS) * ( qSqrt( 1/(qPow( (valST/QRS), (1/3) )) ) )
                - 11*tRS * ( qPow( (valST/QRS), (1/3) ) );
    }
    else
    {
        KCR = 22 * QRS * qSqrt( 1/(qPow( (valST/QRS), (1/3) )) ) - 0.5*tRS * qPow( (valST/QRS), (1/3) );
    }
    //++++++++++++++++

    //+++++КСО КДО+++++
    double KDO, KSO;
    KDO = 4/3 * M_PI * qPow( KDR, 3 );
    KSO = 4/3 * M_PI * qPow( KCR, 3 );
    //+++++++++++++++++

    //+++++ФВ++++
    double FV;
    FV = ((KDO-KSO)/KDO) * 100;
    //+++++++++++

    //+++++K+++++
    double K;
    K = -4.518 + 0.02*FV + 0.037*SDANN +0.049*valLFHF - 0.019*varQT;
    //+++++++++++

    if(Method == ekp) {
        //++++Диагноз+++++
        Diagnosis = "Вывести диагноз невозможно";
        if( ((k * qSqrt(valR) - varQT)>0) && FV<50 )
            Diagnosis = "<p>Имеется желудочная тахикардия и экстрасистолия,"
                        "что характеризует гемодинамически значимую аритмию.</p>"
                        "Необходимо провести дефибрилляцию.";
        if( ((k * qSqrt(valR) - varQT)>0) && FV>=50 )
            Diagnosis = "<p>Имеется желудочная тахикардия и экстрасистолия,"
                        "что характеризует гемодинамически значимую аритмию.</p>"
                        "Необходимо провести реваскуляризацию.";
        if( ((k * qSqrt(valR) - varQT)<=0) )
            if(K >= 2.5)
                Diagnosis = "<p>Тяжелый аритмический синдром.</p>"
                            "Необходимо провести реваскуляризацию.";
        //++++++++++++++++
        ui->textEdit_2->clear();
        ui->textEdit_2->setText(Diagnosis);
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    Method = ns;

    QVector< QVector<bool> > Array;
    QString diag, buff;

    Array.resize(12);
    for(int i=0; i<12; i++)
        Array[i].resize(16);

    for(int i=0; i<12; i++)
        Array[i][0] = true;

    bool D1 = false, D2 = false, D3 = false, bBuff = true;

    if(rand1 == 1)
        for(int i=0; i<12; i++) {
            Array[i][0] = true;
            D1 = true;
            rand1 = 2;
        }
    else if(rand1 == 2)
        for(int i=0; i<10; i++)
            for(int j=0; j<11; j++){
                Array[i][j] = true;
                D2 = true;
                rand1 = 3;
            }
    else if(rand1 == 3) {
        D1 = true;
        D2 = true;
        rand1 = 1;
    }

//    for(int i=0; i<12; i++)
//        bBuff = bBuff & Array[i][0];
//    D1 = bBuff; //D1

//    bBuff = true;
//    for(int i=0; i<10; i++)
//        for(int j=0; j<11; j++)
//            bBuff = bBuff || Array[i][j];
//    D2 = bBuff; //D2

    //D3 = !D1 & !D2;

    if(D1 == true && D2 == false) diag = "<p>Подозрение на боковой инфаркт миокарда в рубцовой стадии. Затронутые области и глубина поражения ИМ:</p>"
                                        "<p>1) перегородочная область - субэпикардиальный ИМ;</p>"
                                        "<p>2) передняя область - субэпикардиальный ИМ;</p>"
                                        "<p>3) боковая верхняя область - трансмуральный ИМ;</p>"
                                        "<p>4) боковая нижняя область - трансмуральный ИМ.</p>";
    if(D2 == true && D1 == false) diag = "<p>Подозрение на боковой инфаркт миокарда в рубцовой стадии. Затронутые области и глубина поражения ИМ:</p>"
                                        "<p>1) боковая верхняя область - субэпикардиальный ИМ;</p>"
                                        "<p>2) боковая нижняя область - трансмуральный ИМ</p>";
    if(D1 == true && D2 == true) diag = "<p>Подозрение на переднеперегородочный инфаркт миокарда с переходом на верхушку в острейшей стадии. Затронутые области и глубина поражения ИМ:</p>"
                                       "<p>1) перегородочная область - субэпикардиальный ИМ;</p>"
                                       "<p>2) передняя область - субэпикардиальный ИМ.</p>";

    //buff = ui->textEdit_2->toPlainText();
    ui->textEdit_2->clear();
    ui->textEdit_2->setText(diag); //вывод диагноза

    //ОТСЮДА НАЧАТЬ ПИЛИТЬ ФУНКЦИЮ

    //if(QString(fileName[fileName.length()-5]) == "1") { //привязка к файлу
        //diag = ;
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

        if ((firstCount>=secondCount-3) || (firstCount<0) || (secondCount - firstCount < 3000)) {
            showError("Не верно введены значения отсчетов");
            return;
        }

        selectedLead = ui->comboBox->currentIndex()+1;

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
        //по завершению работы TCalc запускается newTaskEMD
        connect(TCalc, SIGNAL(finished(QVector<double>) ), this, SLOT(newTaskEMD(QVector<double>) ));

        TCalc->moveToThread(Thread);

        Thread->start();
    //}
}
