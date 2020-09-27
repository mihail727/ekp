#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QPixmap"
#include <QtCore/qmath.h>

#include <Methods/cqrs.h>
#include <Methods/emd.h>
#include <Methods/methodfacility.h>
#include <Methods/method.h>

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

void MainWindow::showError(QString st)
{
    QPixmap MainIcon(":/resource/img/icons8-plus-48.png");
    QMessageBox msgBox(QMessageBox::NoIcon, tr("Ошибка"), st, QMessageBox::Ok);

    msgBox.setWindowIcon(MainIcon);
    msgBox.exec();
//--------------------------------------------------------------------------------------------
}

void MainWindow::GenerateDiagnosis()
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
    if(ui->checkBoxBlockade->isChecked()) //Блокада Гиса
    {
        if(ui->radioButtonBlockadeRight->isChecked()) Gysa = right;
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

    if(currentMethod == ekp) {
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

void MainWindow::on_BtnOpenFile_clicked()
{
    try
    {
        DataFile::OpenFileDlg();
    } catch (...) {
        showError("Ошибка на этапе работы с data file.");
        return;
    }
}

void MainWindow::on_BtnStart_clicked()
{    
    ui->BtnStart->setEnabled(false);

    DataFile* dataFile = new DataFile();

    QThread *thread = QThread::create([dataFile]{ dataFile->GenerateArrayData(); });
    thread->start();
    while(!thread->isFinished())
        QCoreApplication::processEvents();

    try
    {
        CheckAllFeilds_isNoEmpty(*dataFile);
    } catch (...) {
        showError("Ошибка на этапе проверки входных данных.");
        ui->BtnStart->setEnabled(true);
        return;
    }

    try
    {
        QVector<double> currentArrayData = dataFile->GenerateCurrentArray(SamplesCount, CurrentAbduction);

        ChartControl* chartControl = new ChartControl(ui->chart1);
        chartControl->ClearChart();

        QVector<double> formattedArrayTime = chartControl->getTimeArray(Hertz, SamplesCount);

        MethodFacility* methodFacility = new MethodFacility();
        Method* method = methodFacility->Create(currentMethod);
        if(method != nullptr)
            method->doCalc(currentArrayData, 0, formattedArrayTime.length(), *chartControl, formattedArrayTime);
        else
            chartControl->AddGraphic(formattedArrayTime, currentArrayData, "lsLine");

        GenerateDataConfig(chartControl->topsQRS, chartControl->topsQ, chartControl->topsS);
    } catch (...) {
        showError("Ошибка на этапе генерации данных текущего метода.");
        ui->BtnStart->setEnabled(true);
        return;
    }

    ui->BtnStart->setEnabled(true);
}

void MainWindow::CheckAllFeilds_isNoEmpty(DataFile& dataFile)
{
    if (ui->lineEditTime->text() == "" || ui->lineEditTime->text().isNull()
            ||  ui->lineEditTime->text().toInt() == 0 ||  ui->lineEditTime->text().toInt() < 0)
        throw QException();

    time = ui->lineEditTime->text().toInt() * timeFactor;

    SamplesCount = (int)(time * Hertz);
    if(SamplesCount > dataFile.MaxArrayDataSize)
        SamplesCount = dataFile.MaxArrayDataSize;
    if(SamplesCount < 1000)
        SamplesCount = 1000;
}

void MainWindow::GenerateDataConfig(QVector<double>& topsQRS, QVector<double>& topsQ,
                                    QVector<double>& topsS)
{
    if(topsQRS.length() > 2)
    {
        //Пульс
        double difValue = 0;
        for(int i=0; i<topsQRS.size()-1; i++)
            difValue = difValue + abs(topsQRS[i] - topsQRS[i+1]);
        difValue = difValue / (topsQRS.size()-1);
        int pulse;
        pulse = int( (1/difValue*1000)*60 );
        difValue = int(difValue);

        //SDANN
        int summ = 0;
        for(int i=0; i< topsQRS.size()-1; i++)
            summ = summ + int( qPow( abs(topsQRS[i] - topsQRS[i+1]) - difValue, 2) );
        summ = int( ( 1.0/(topsQRS.size()-1) ) * qSqrt(summ) );

        //varQt
        double max = topsQRS[0];
        double min = max;
        for(int i=0; i<topsQRS.size(); i++)
        {
            if(max < topsQRS[i])
                max = topsQRS[i];
            if(min > topsQRS[i])
                min = topsQRS[i];
        }
        double varQt;
        varQt = qCeil( 100/(max/min) );

        //t(RS)
        int difRS = 0;
        for(int i=0; i<topsQRS.size(); i++)
        {
            difRS = difRS + int(topsS[i] - topsQRS[i]);
        }
        difRS = difRS / topsQRS.size();

        //t(QR)
        int difQR = 0;
        for(int i=0; i<topsQRS.size(); i++)
        {
            difQR = difQR + int(topsQRS[i] - topsQ[i]);
        }
        difQR = difQR / topsQRS.size();

        //t(QRS)
        int difQRS = 0;
        for(int i=0; i<topsQRS.size(); i++)
        {
            difQRS = difQRS + int(topsS[i] - topsQ[i]);
        }
        difQRS = difQRS / topsQRS.size();

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

        GenerateDiagnosis();

    } else {
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

void MainWindow::on_comboBoxAbduction_currentIndexChanged(int index)
{
    CurrentAbduction = index;
}

void MainWindow::on_comboBoxTime_currentIndexChanged(int index)
{
    switch (index)
    {
        case 0:
            timeFactor = 0.001;
            break;

        case 1:
            timeFactor = 1;
            break;

        case 2:
            timeFactor = 60;
            break;

        case 3:
            timeFactor = 3600;
            break;
    }
}

void MainWindow::on_comboBoxMethod_currentIndexChanged(int index)
{
    currentMethod = (eMethod)index;
}
