#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QPixmap"

#include <Methods/hflf.h>
#include <Methods/calc.h>
#include <Methods/cqrs.h>

/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
                    /*ФУНКЦИИ ПЕРВОЙ ВАЖНОСТИ*/
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

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
    connect(qrs, SIGNAL(sendQRSValues(QVector<double>, QVector<double>,
                                      QVector<double>, QVector<double>, QVector<double>,
                                      QVector<double>, QVector<double>)  ),
            this, SLOT(drawQRS(QVector<double>, QVector<double>,
                               QVector<double>, QVector<double>, QVector<double>,
                               QVector<double>, QVector<double>)));

    //по завершению начинается расчет Пульса, SDANN, varQT
    connect(qrs, SIGNAL(sendQRSValues(QVector<double>, QVector<double>,
                                      QVector<double>, QVector<double>, QVector<double>,
                                      QVector<double>, QVector<double>)  ),
            this, SLOT(CalculateSomeProc(QVector<double>, QVector<double>,
                                         QVector<double>, QVector<double>, QVector<double>,
                                         QVector<double>, QVector<double>) ));

    thread->start();
}

void MainWindow::CalculateSomeProc(QVector<double> ArrPanTom, QVector<double> RValues,
                                   QVector<double>ArrMain, QVector<double> QValues, QVector<double> SValues,
                                   QVector<double> a, QVector<double> b)
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

        chartTitle->setText("Средняя длительность кардиоцикла: -- ; Пульс: --");

        ui->chart1->replot();
    }
}

void MainWindow::drawQRS(QVector<double> ArrPanTom, QVector<double> RValues,
                         QVector<double> ArrMain, QVector<double> QValues, QVector<double> SValues,
                         QVector<double> PValues, QVector<double> TValues)
//Вывод графика QRS
{
    ui->chart2->addGraph(); //graph0

    ui->chart2->graph(0)->setPen(QPen(Qt::black));

    //формирование графика QRS
    QVector<double> ArrPanTom_x, ArrPanTom_y;
    int j=0;
    for(int i=firstCount; i<ArrPanTom.size(); i++) {
        ArrPanTom_x.push_back(i);
        ArrPanTom_y.push_back(ArrPanTom[i]);
    }

    ui->chart2->graph(0)->setData(ArrPanTom_x, ArrPanTom_y);
    ui->chart2->graph(0)->rescaleAxes();

    QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
    ui->chart2->xAxis->setTicker(fixedTicker);
    ui->chart2->yAxis->setTicker(fixedTicker);
    fixedTicker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples);
    fixedTicker->setTickCount(16);

    ui->chart2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

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

    ui->chart1->graph(2)->setData(RValues, RValues_y);
    ui->chart1->graph(3)->setData(QValues, QValues_y);
    ui->chart1->graph(4)->setData(SValues, SValues_y);
    ui->chart1->graph(5)->setData(PValues, PValues_y);
    ui->chart1->graph(6)->setData(TValues, TValues_y);

    ui->chart1->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QColor(Qt::darkGreen), QColor(Qt::green), 7));
    ui->chart1->graph(3)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QColor(Qt::darkGreen), QColor(Qt::green), 7));
    ui->chart1->graph(4)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QColor(Qt::darkGreen), QColor(Qt::green), 7));
    ui->chart1->graph(5)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QColor(Qt::darkGreen), QColor(Qt::red), 7));
    ui->chart1->graph(6)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QColor(Qt::darkGreen), QColor(Qt::red), 7));

    ui->chart1->graph(3)->setLineStyle(QCPGraph::lsNone);
    ui->chart1->graph(4)->setLineStyle(QCPGraph::lsNone);
    ui->chart1->graph(5)->setLineStyle(QCPGraph::lsNone);
    ui->chart1->graph(6)->setLineStyle(QCPGraph::lsNone);

    ui->chart1->yAxis->setRange( ui->chart1->yAxis->range().lower-100, ui->chart1->yAxis->range().upper+300 );

    ui->chart1->axisRect()->setRangeZoom(Qt::Horizontal);

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
    ui->textEdit->insertPlainText(QString::number(a[0]));
}

/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
                    /*ФУНКЦИИ ВТОРОЙ ВАЖНОСТИ*/
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    //Вывод координат возле курсора
    if(object == ui->chart1)
    {
        if(event->type() == QEvent::MouseMove)
        {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if( mouse_event->pos().x() < 30 || mouse_event->pos().y() > (ui->chart1->height() - 30) )
            {
                Coordinates->setVisible(false);
                ui->chart1->replot();
            }
            else {
                double x, y;
                x = int(this->ui->chart1->xAxis->pixelToCoord(mouse_event->pos().x()));
                y = int(this->ui->chart1->yAxis->pixelToCoord(mouse_event->pos().y()));

                QPoint coord;
                coord.setX(int( x + (ui->chart1->xAxis->pixelToCoord(40) - ui->chart1->xAxis->pixelToCoord(0)) ));
                coord.setY(int( y + (ui->chart1->yAxis->pixelToCoord(-20) - ui->chart1->yAxis->pixelToCoord(0)) ));


                Coordinates->setText(QString::number(x) + ":" + QString::number(y));
                Coordinates->setVisible(true);
                Coordinates->position->setCoords(coord);

                ui->chart1->replot();
            }
            return false;
        }
    }
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

    ui->chart1->installEventFilter(this);
    ui->chart1->setMouseTracking(true);

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

    //Значения X Y возле курсора
    Coordinates = new QCPItemText(ui->chart1);
    Coordinates->setFont(QFont("Tahoma", 10));
    Coordinates->setBrush(Qt::white);
    Coordinates->setVisible(false);

    //Иконка приложения
    QLabel *WinIconTitle = new QLabel();
    QIcon WinIcon(":/resource/img/icons8-plus-48.png");
    QPixmap pixmapWinIcon = WinIcon.pixmap(QSize(20, 20));
    WinIconTitle->setPixmap(pixmapWinIcon);
    ui->toolBar_2->addWidget(WinIconTitle);

    //title приложения
    QLabel *WinTitle = new QLabel("Экстренное оказание кардиологической помощи");
    WinTitle->setStyleSheet("QLabel{ font-size: 8pt;"
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
    fixedTicker->setTickCount(16);

    ui->chart1->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->chart1->replot();
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
