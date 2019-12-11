#include "cqrs.h"
#include <QVector>
#include <iostream>
#include <QDebug>

static QVector <double> masspoint1(1000000);

void low (const QVector<double> &masspoint3, int firstCount, int secondCount) // фильтр нижних частот
{
    int n = secondCount- firstCount;
    QVector <double> filter1(n);
    QVector <double> filter2(n);

    filter1 = masspoint3;
    for (int i = 12; i < n; i++)
    {
        filter2[i] =(2 * filter2[i - 1] - filter2[i - 2] + (filter1[i] - 2 * filter1[i - 6] + filter1[i - 12]) / 32);
    }
    masspoint1 = filter2;
}

void high(const QVector<double> &masspoint3, int firstCount, int secondCount)// фильтр верхних частот
{

    int n = secondCount- firstCount;
    QVector <double> filter1(n);
    QVector <double> filter2(n);


    filter1 = masspoint3;


    for (int i = 32; i < n; i++)
    {
        filter2[i]= (filter2[i - 1] - filter1[i] / 32 + filter1[i - 16] - filter1[i - 17] + filter1[i - 32] / 32);
    }


    masspoint1 = filter2;
}

void differentiation(const QVector<double> &masspoint3, int firstCount, int secondCount)//функция дифференцирования
{
    int n = secondCount- firstCount;
    QVector <double> filter1(n);
    QVector <double> filter2(n);


    filter1 = masspoint3;


    for (int i = 4; i < n; i++)
    {
        filter2[i] = (2*filter1[i] + filter1[i-1] - filter1[i-3] - 2* filter1[i-4])/8;
    }

    masspoint1 = filter2;
}

void square(const QVector<double> &masspoint3, int firstCount, int secondCount)//функция возведения в квадрат
{
    int n = secondCount- firstCount;
    QVector <double> filter1(n);

    for (int i = 0; i < n; i++)
    {
        filter1[i] = masspoint3[i] * masspoint3[i];
    }

    masspoint1 = filter1;
}

QVector<double> integration(const QVector<double> &masspoint3, int firstCount, int secondCount)// интегрирование
{
    int N=100;
    //   N = Convert.ToInt32(comboBox1.Text); измени сам. с формы должна "ширина окна" передаваться
    int n = secondCount- firstCount;
    QVector <double> filter1(n);
    QVector <double> filter2(n);


    for (int i = 0; i < n; i++)
    {
        filter1[i] = masspoint3[i];
    }

    for (int i = 0; i < n; i++)
    {
        filter2[i] = 0;
    }


    for (int i = N; i < n; i++)
    {
        double summa = 0;
        for (int j = N; j > 0;  j--)
        {
            summa = summa + filter1[i-(j)];
            //qDebug()<< j << " ";
        }
        filter2[i] = summa / N;
    }


    masspoint1 = filter2;
    return masspoint1;
}

QVector<double> topsQRS(const QVector<double> &mas1, const QVector<double> &mas)
{
    int k = 0;
    double max1 = -10000;
    for (int i = 0; i < mas.size()-1; i++)
    {
        if (max1 < mas[i])
        {
            k=i;
            max1 = mas[i];
        }
    }
    auto max = 0;
    int n = mas.size();
    QVector<double> res(n);
    for (int i = 0; i< n; i ++)
    {
        res[i] = 0;
    }
        for (int i=99; i < n; i++)
        {

            while ((mas[i] >= max1/2) && (i < n-1))
            {
                if (abs(mas1[max]) - abs(mas1[0]) < abs(mas1[i-99]) - abs(mas1[0]))
                    max = i-99;
                i++;

            }
            if (res.indexOf(max)==-1)
                res[i] = max;
            max=0;
        }
    res.removeAll(0);
    return res;
}
QVector<double> topsQ(const QVector<double> &mas1, const QVector<double> &mas)
{
    int n = mas.size();
    QVector<double> res(n);
    for (int i = 0; i< n; i ++)
    {
        res[i] = 0;
    }

    double min = 0;

    for (int j = 0; j < mas.size(); j++)
    {
        if (mas1[j]>0)
        {
            min = mas1[j];
            for (int i=mas[j] - 55; i < mas[j]; i++)
            {

                while ((i < mas[j]))
                {
                    if (mas1[j]+ abs(mas1[min]) > mas1[j] + abs(mas1[i]))
                        min = i;
                    i++;

                }
                if (res.indexOf(min)==-1)
                    res[j]= min;
            }
        }

        else
        {
            double min = 0;

            for (int j = 0; j < mas.size(); j++)
            {

                min = mas[j];
                for (int i=mas[j] - 55; i < mas[j]; i++)
                {

                    while ((i < mas[j]))
                    {
                        if (mas1[j]-mas1[min] < mas1[j]- mas1[i])
                            min = i;
                        i++;

                    }
                    if (res.indexOf(min)==-1)
                        res[j]= min;
                }
            }
        }
    }
    res.removeAll(0);
    return res;

}
QVector<double> topsS(const QVector<double> &mas1, const QVector<double> &mas)
{

    int n = mas.size();
    QVector<double> res(n);
    for (int i = 0; i< n; i ++)
    {
        res[i] = 0;
    }
    double min = 0;

    for (int j = 0; j < mas.size(); j++)
    {
        if (mas1[j]>0)
        {
            min = mas[j];
            for (int i=mas[j] + 100; i > mas[j]; i--)
            {

                while ((i > mas[j]))
                {
                    if (mas1[j]-mas1[min] < mas1[j]- mas1[i])
                        min = i;
                    i--;

                }
                if (res.indexOf(min)==-1)
                    res[j]= min;
            }

        }
        else {
            min = mas[j];
            for (int i=mas[j] + 100; i > mas[j]; i--)
            {

                while ((i > mas[j]))
                {
                    if (mas1[j]-mas1[min] < mas1[j]- mas1[i])
                        min = i;
                    i--;
                }
                if (res.indexOf(min)==-1)
                    res[j]= min;
            }
        }
    }
    res.removeAll(0);
    return res;

}

QVector<double> topsP(const QVector<double> &mas1, const QVector<double> &mas)
{
    int n = mas.size();
    QVector<double> res(n);
    for (int i = 0; i< n; i ++)
    {
        res[i] = 0;
    }
    double max = 0;

    for (int j = 0; j < mas.size(); j++)
    {

        max = mas[j];
        for (int i= mas[j] - 250; i < mas[j]; i++)
        {

            while ((i < mas[j]))
            {
                if (mas1[max] < mas1[i])
                    max = i;
                i++;
            }
            if (res.indexOf(max)==-1)
                res[j]= max;
        }
    }
    res.removeAll(0);
    return res;
}

QVector<double> topsT(const QVector<double> &mas1, const QVector<double> &mas)
{
    int n = mas.size();
    QVector<double> res(n);
    for (int i = 0; i< n; i ++)
    {
        res[i] = 0;
    }
    double max = 0;

    for (int j = 0; j < mas.size(); j++)
    {

        max = mas[j];
        for (int i= mas[j] - 250; i < mas[j]- 60; i++)
        {

            while ((i < mas[j]))
            {
                if (mas1[max] < mas1[i])
                    max = i;
                i++;
            }
            if (res.indexOf(max)==-1)
                res[j]= max;
        }
    }
    res.removeAll(0);
    return res;

}

QVector<double> beginP(const QVector<double> &mas)
{
    int n = mas.size();
    QVector<double> res(n);
    for (int i = 0; i< n; i ++)
    {
        res[i] = mas[i]-70;
    }

    res.removeAll(0);
    return res;

}



QVector<double> Proizvodnaya(const QVector<double> &mas)
{
    int n = mas.size();
    QVector<double> res(n);
    for (int i = 3; i< n; i ++)
    {
        res[i] = mas[i]-mas[i-3];
    }

    res.removeAll(0);
    return res;

}


void cQRS::doCalc(const QVector<double> &mas, int firstCount, int secondCount)
{
    cData Data;
    QList<cData> List_for_graphic, List_for_calc;

    QVector<double> result(secondCount-firstCount);
    QVector<double> result1(secondCount-firstCount);
    QVector<double> result2(secondCount-firstCount);
    QVector<double> result3(secondCount-firstCount);
    QVector<double> result4(secondCount-firstCount);
    QVector<double> result5(secondCount-firstCount);
    QVector<double> dif(secondCount);
    low(mas, firstCount, secondCount );
    high(masspoint1, firstCount, secondCount );
    differentiation(masspoint1, firstCount, secondCount );
    square(masspoint1, firstCount, secondCount );
    result = integration(masspoint1, firstCount, secondCount );
    result1 = topsQRS(mas, result);
    result2 = topsQ(mas, result1);
    result3 = topsS(mas, result1);
    result4 = topsP(mas, result2);
    result5 = beginP(result4);

    differentiation(mas, firstCount, secondCount );
    dif = Proizvodnaya(mas);

    for (int i=0; i< dif.size()-3; i++)
        dif[i] = dif[i+3];

/****************************************/
/*ЭТАП НАСТРОЙКИ ВЕКТОРОВ И ИХ ОТПРАВКИ*/
/****************************************/
    Data.Array_X = result1;
    List_for_calc.push_back(Data);
//    Data.Type = cData::Point;
//    Data.color_Line = QColor(255,0,0);
//    Data.color_Point = QColor(255,255,0);
//    Data.color_Frame = QColor(255,255,255);
    Data.Clear();

    Data.Array_X = result2;
    List_for_calc.push_back(Data);
    Data.Clear();

    Data.Array_X = result3;
    List_for_calc.push_back(Data);
    Data.Clear();

    Data.Array_X = result1;
    //Data.Array_Y = result1;
    List_for_graphic.push_back(Data);
    Data.Clear();

    Data.Array_X = mas;
    //Data.Array_Y = mas;
    List_for_graphic.push_back(Data);
    Data.Clear();

    Data.Array_X = result2;
    //Data.Array_Y = mas;
    List_for_graphic.push_back(Data);
    Data.Clear();

    Data.Array_X = result3;
    //Data.Array_Y = mas;
    List_for_graphic.push_back(Data);
    Data.Clear();

    Data.Array_X = result4;
    //Data.Array_Y = mas;
    List_for_graphic.push_back(Data);
    Data.Clear();

    Data.Array_X = result5;
    //Data.Array_Y = mas;
    List_for_graphic.push_back(Data);
    Data.Clear();

    emit sendValues_for_calculate(List_for_calc);
    //emit sendValues_for_drawGraphic(List_for_graphic);
    emit finished();
}
