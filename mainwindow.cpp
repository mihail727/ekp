#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QPixmap"
#include <QtCore/qmath.h>

#include <Methods/calc.h>
#include <Methods/cqrs.h>
#include <Methods/emd.h>
#include <Methods/fazagrov.h>

static int rand1 = 1;

/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
                    /*ФУНКЦИИ ПЕРВОЙ ВАЖНОСТИ*/
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

void MainWindow::newTask(QVector<double> Array)
{
    QThread *thread = new QThread;
    connect(thread, SIGNAL(finished() ), thread, SLOT(deleteLater() ));

    QObject *Object = nullptr;

    switch (Method) {
    case emd: {
        cEMD *emd = new cEMD();
        Object = emd;

        connect(thread, &QThread::started, emd, [=] {
            emd->doCalc(Array, firstCount, secondCount);
        });
        break;
    }
    case ekp: {
        cQRS *qrs = new cQRS();
        Object = qrs;

        connect(thread, &QThread::started, qrs, [=] {
            qrs->doCalc(Array, firstCount, secondCount);
        });

        //по завершению начинается расчет Пульса, SDANN, varQT
//        connect(qrs, SIGNAL(sendValues_for_calculate(QList<cData>)),
//                this, SLOT(CalculateSomeProc(QList<cData>)));
//      //по завершению работы newTaskQRS
//      connect(qrs, SIGNAL(finished()), this, SLOT(Calc_Diag()));
        break;
    }
    case ns: {
        cQRS *qrs = new cQRS();
        Object = qrs;

        connect(thread, &QThread::started, qrs, [=] {
            qrs->doCalc(Array, firstCount, secondCount);
        });

        //по завершению начинается расчет Пульса, SDANN, varQT
//        connect(qrs, SIGNAL(sendValues_for_calculate(QList<cData>)),
//                this, SLOT(CalculateSomeProc(QList<cData>)));
//      //по завершению работы newTaskQRS
//      connect(qrs, SIGNAL(finished()), this, SLOT(Calc_Diag()));
        break;
    }
    case fazagrov: {
        Fazagrov *faz = new Fazagrov();
        Object = faz;

        connect(thread, &QThread::started, faz, [=] {
            faz->doCalc(Array, firstCount, secondCount);
        });
        break;
    }}

    Object->moveToThread(thread);

    connect(Object, SIGNAL(finished()), thread, SLOT(quit()));
    connect(Object, SIGNAL(finished() ), Object, SLOT(deleteLater() ));

    //по завершению Object выодится график
    connect(Object, SIGNAL(draw_graphic(QList<cData>)),
            this, SLOT(draw_graphic(QList<cData>)));

    thread->start();
}

void MainWindow::draw_graphic(QList<cData> List)
{
    ui->chart1->clearGraphs();
    ui->chart2->clearGraphs();
    ui->chart3->clearGraphs();

    QCustomPlot *chart = nullptr;
    switch (Method) {
    case ekp:
        chart = ui->chart1;
        break;
    case ns:
        chart = ui->chart3;
        break;
    case emd:
        chart = ui->chart2;
        break;
    case fazagrov:
        chart = ui->chart3;
        break;
    }

    int numOfGraph = -1;
    for(int i=0; i<List.size(); i++)
    {
        numOfGraph++;

        chart->addGraph(); //new graph
        chart->graph(numOfGraph)->setData(List[i].Array_X, List[i].Array_Y);
        chart->graph(numOfGraph)->setPen(QPen(List[i].Line));

        if(List[i].Type == cData::Line)
            chart->graph(numOfGraph)->setLineStyle(QCPGraph::lsLine);
        else {
            chart->graph(numOfGraph)->setLineStyle(QCPGraph::lsNone);
            chart->graph(numOfGraph)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,
                                                                      List[i].color_Frame,
                                                                      List[i].color_Point, 7));
        }
    }

    QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
    chart->xAxis->setTicker(fixedTicker);
    chart->yAxis->setTicker(fixedTicker);

    fixedTicker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples);
    fixedTicker->setTickCount(16);

    chart->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    chart->axisRect()->setRangeZoom(Qt::Horizontal);

    chart->yAxis->setRange( ui->chart3->yAxis->range().lower-100, ui->chart3->yAxis->range().upper+300 );
    chart->rescaleAxes();
    chart->replot();
}

void MainWindow::CalculateSomeProc(QList<cData> List)
//Расчет дополнительных параметров (Пульс, SDANN, varQT ...)
{
//    if(RValues.size() > 2)
//    {
//        //Пульс
//        double difValue = 0;
//        for(int i=0; i<RValues.size()-1; i++)
//            difValue = difValue + abs(RValues[i] - RValues[i+1]);
//        difValue = difValue / (RValues.size()-1);
//        int pulse;
//        pulse = int( (1/difValue*1000)*60 );
//        difValue = int(difValue);
//        //++++++++++++++++++++++++++++++++++++

//        //SDANN
//        int summ = 0;
//        for(int i=0; i< RValues.size()-1; i++)
//            summ = summ + int( qPow( abs(RValues[i] - RValues[i+1]) - difValue, 2) );
//        summ = int( ( 1.0/(RValues.size()-1) ) * qSqrt(summ) );
//        //+++++++++++++++++++++++++++++++++++++++++++++++++++++

//        //varQt
//        double max = RValues[0];
//        double min = max;
//        for(int i=0; i<RValues.size(); i++)
//        {
//            if(max < RValues[i])
//                max = RValues[i];
//            if(min > RValues[i])
//                min = RValues[i];
//        }
//        double varQt;
//        varQt = qCeil( 100/(max/min) );
//        //+++++++++++++++++++++++++++++

//        //t(RS)
//        int difRS = 0;
//        for(int i=0; i<RValues.size(); i++)
//        {
//            difRS = difRS + int(SValues[i] - RValues[i]);
//        }
//        difRS = difRS / RValues.size();
//        //+++++++++++++++++++++++++++++

//        //t(QR)
//        int difQR = 0;
//        for(int i=0; i<RValues.size(); i++)
//        {
//            difQR = difQR + int(RValues[i] - QValues[i]);
//        }
//        difQR = difQR / RValues.size();
//        //+++++++++++++++++++++++++++++

//        //t(QRS)
//        int difQRS = 0;
//        for(int i=0; i<RValues.size(); i++)
//        {
//            difQRS = difQRS + int(SValues[i] - QValues[i]);
//        }
//        difQRS = difQRS / RValues.size();
//        //

//        //Вывод данных
//        ui->lineEdit_3->setText(QString::number(summ)); //SDANN
//        ui->lineEdit_4->setText(QString::number(varQt)); //varQt
//        ui->lineEdit_5->setText(QString::number(difQR)); //t(QR)
//        ui->lineEdit_6->setText(QString::number(difRS)); //t(RS)
//        ui->lineEdit_7->setText(QString::number(difValue)); //R, mc
//        ui->lineEdit_10->setText(QString::number(difQRS)); //t(QRS)

//        chartTitle->setText("Средняя длительность кардиоцикла: " + QString::number(difValue) + " (мс); "
//                            + "Пульс: " + QString::number(pulse));
//        chartTitle->setVisible(true);

//        ui->chart1->replot();
//        //
//    }
//    else {
//        ui->lineEdit_3->clear(); //SDANN
//        ui->lineEdit_4->clear(); //varQt
//        ui->lineEdit_5->clear(); //t(QR)
//        ui->lineEdit_6->clear(); //t(RS)
//        ui->lineEdit_7->clear(); //R, mc
//        ui->lineEdit_10->clear(); //t(QRS)
//        ui->textEdit_2->clear(); //Диагноз

//        chartTitle->setText("Средняя длительность кардиоцикла: -- ; Пульс: --");

//        ui->chart1->replot();
//    }
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

void MainWindow::on_pushButton_clicked()
//Начало расчета диагноза
{    
    Method = ekp;
    start_Calc();
    ui->tabWidget->setCurrentIndex(0);
}

void MainWindow::on_pushButton_3_clicked()
//Fazagrov
{
    Method = fazagrov;
    start_Calc();
    ui->tabWidget->setCurrentIndex(2);
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
    ui->tabWidget->setCurrentIndex(0);

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

    start_Calc();
}

void MainWindow::on_pushButton_4_clicked()
//emd
{
    Method = emd;
    start_Calc();
    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::start_Calc()
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

    //Обработка файла + создание массива с исходными данными

    QThread *Thread = new QThread;
    Calc *TCalc = new Calc();

    connect(TCalc, SIGNAL(sendError(QString)), this, SLOT(showError(QString)));

    connect(TCalc, SIGNAL(finished(QVector<double>) ), Thread, SLOT(quit() ));
    connect(TCalc, SIGNAL(finished(QVector<double>) ), TCalc, SLOT(deleteLater() ));
    connect(Thread, SIGNAL(finished() ), Thread, SLOT(deleteLater() ));

    connect(Thread, &QThread::started, TCalc, [=] {
        TCalc->doCalc(fileName, selectedLead, firstCount, secondCount);
    });

    connect(TCalc, SIGNAL(finished(QVector<double>) ), this, SLOT(newTask(QVector<double>) ));

    TCalc->moveToThread(Thread);

    Thread->start();
}
