#include "cqrs.h"
#include <QVector>
#include <iostream>
#include <QDebug>

#include <math.h>


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
void differentiation2(const QVector<double> &masspoint3, int firstCount, int secondCount)//функция дифференцирования вариант 2
{
    int n = secondCount- firstCount;
    QVector <double> filter1(n);
    QVector <double> filter2(n);


    filter1 = masspoint3;


    for (int i = 4; i < n-5; i+=5)
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
    double max1 = -10000;
    for (int i = 0; i < mas.size()-1; i++)
    {
        if (max1 < mas[i])
        {
            max1 = mas[i];
        }
    }
    double max = 0;
    int n = mas.length();
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
        for (int i= mas[j] + 60; i < mas[j]+ 250; i++)
        {

            while ((i < mas[j]+250) && (i != mas1.size()))
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

QVector<double> beginP(const QVector<double> &diff, const QVector<double> mas)
{
    int n = mas.size();
    QVector<double> res(n);
    double max;

    for (int i = 0; i< mas.size(); i ++)
    {
        max = diff[i];
        int  j = mas[i];

        while (diff[j]> max*0.1)
        {
            if (diff[j] > max)
                max = diff[j];
            j--;
        }
        res[i] = j-40;
    }


    res.removeAll(0);
    return res;

}
QVector<double> endsP(const QVector<double> &diff, const QVector<double> mas)
{
    int n = mas.size();
    QVector<double> res(n);
    double max;

    for (int i = 0; i< mas.size(); i ++)
    {
        max = diff[i];
     int  j = mas[i];

        while (diff[j]> max*0.2)
        {
            if (diff[j] > max)
                max = diff[j];
            j++;
        }
        res[i] = j+40;
    }
    res.removeAll(0);
    return res;
}
QVector<double> beginT(const QVector<double> &diff,const QVector<double> &mas )
{
    int n = mas.size();
    QVector<double> res(n);
    double max;
    for (int i = 0; i< mas.size(); i ++)
    {
        max = diff[i];
        int  j = mas[i];

        while (diff[j]> max*0.1)
        {
            if (diff[j] > max)
                max = diff[j];
            j--;
        }
        res[i] = j-40;
    }


    res.removeAll(0);
    return res;
}
QVector<double> endsT(const QVector<double> &diff, const QVector<double> mas)
{
    int n = mas.size();
    QVector<double> res(n);
    double max;

    for (int i = 0; i< mas.size(); i ++)
    {
        max = diff[i];
     int  j = mas[i];

        while (diff[j]> max*0.2)
        {
            if (diff[j] > max)
                max = diff[j];
            j++;
        }
        res[i] = j+40;
    }
    res.removeAll(0);
    return res;
}

QVector<double> Proizvodnaya(const QVector<double> &mas)
{
    int n = mas.size();
    QVector<double> res(n);
    for (int i = 40; i< n; i ++)
    {
       res[i] = abs(mas[i]-mas[i-40]);
    }

    res.removeAll(0);
    return res;

}

void cQRS::doCalc(const QVector<double> &mas, int firstCount,
                  int secondCount, ChartControl &chartControl, QVector<double> arrayFomatted)
{
    QVector<double> result(secondCount-firstCount);
    QVector<double> result1(secondCount-firstCount);
    QVector<double> result2(secondCount-firstCount);
    QVector<double> result3(secondCount-firstCount);
    QVector<double> result4(secondCount-firstCount);
    QVector<double> result5(secondCount-firstCount);
    QVector<double> result6(secondCount-firstCount);
    QVector<double> result7(secondCount-firstCount);
    QVector<double> result8(secondCount-firstCount);
    QVector<double> result9(secondCount-firstCount);
    QVector<double> afterFilter(secondCount-firstCount);
    QVector<double> dif(secondCount);
    low(mas, firstCount, secondCount );
    high(masspoint1, firstCount, secondCount );
    afterFilter = masspoint1;
    dif = Proizvodnaya(afterFilter);
    differentiation(masspoint1, firstCount, secondCount );
    square(masspoint1, firstCount, secondCount );

    result = integration(masspoint1, firstCount, secondCount );
    result1 = topsQRS(mas, result);
    result2 = topsQ(mas, result1);
    result3 = topsS(mas, result1);
    result4 = topsP(mas, result2);
    result5 = beginP(dif, result4);
    result6 = endsP(dif, result4);
    result7 = topsT (mas, result3);
    result8 = beginT (dif, result7);
    result9 = endsT (dif, result7);

    chartControl.topsQRS = result1;
    chartControl.topsQ = result2;
    chartControl.topsS = result3;

    chartControl.AddGraphic(arrayFomatted, mas, "lsLine");

    QVector<double> buff_Y, buff_X;
    for(int i = 0; i < result1.length(); i++)
    {
        buff_Y.push_back(mas[result1[i]]);
        buff_X.push_back(arrayFomatted[result1[i]]);
    }
    chartControl.AddGraphic(buff_X, buff_Y, "lsPoint", Qt::green);

    buff_X.clear(); buff_Y.clear();
    for(int i = 0; i < result1.length(); i++)
    {
        buff_Y.push_back(mas[result2[i]]);
        buff_X.push_back(arrayFomatted[result2[i]]);
    }
    chartControl.AddGraphic(buff_X, buff_Y, "lsPoint", Qt::red);

    buff_X.clear(); buff_Y.clear();
    for(int i = 0; i < result1.length(); i++)
    {
        buff_Y.push_back(mas[result3[i]]);
        buff_X.push_back(arrayFomatted[result3[i]]);
    }
    chartControl.AddGraphic(buff_X, buff_Y, "lsPoint", Qt::blue);

    buff_X.clear(); buff_Y.clear();
    for(int i = 0; i < result1.length(); i++)
    {
        buff_Y.push_back(mas[result4[i]]);
        buff_X.push_back(arrayFomatted[result4[i]]);
    }
    chartControl.AddGraphic(buff_X, buff_Y, "lsPoint", Qt::black);

    buff_X.clear(); buff_Y.clear();
    for(int i = 0; i < result1.length(); i++)
    {
        buff_Y.push_back(mas[result5[i]]);
        buff_X.push_back(arrayFomatted[result5[i]]);
    }
    chartControl.AddGraphic(buff_X, buff_Y, "lsPoint", Qt::yellow);

    buff_X.clear(); buff_Y.clear();
    for(int i = 0; i < result1.length(); i++)
    {
        buff_Y.push_back(mas[result6[i]]);
        buff_X.push_back(arrayFomatted[result6[i]]);
    }
    chartControl.AddGraphic(buff_X, buff_Y, "lsPoint", QColor(255, 123, 0));

    buff_X.clear(); buff_Y.clear();
    for(int i = 0; i < result1.length(); i++)
    {
        buff_Y.push_back(mas[result7[i]]);
        buff_X.push_back(arrayFomatted[result7[i]]);
    }
    chartControl.AddGraphic(buff_X, buff_Y, "lsPoint", QColor(255, 0, 230));

    buff_X.clear(); buff_Y.clear();
    for(int i = 0; i < result1.length(); i++)
    {
        buff_Y.push_back(mas[result8[i]]);
        buff_X.push_back(arrayFomatted[result8[i]]);
    }
    chartControl.AddGraphic(buff_X, buff_Y, "lsPoint", QColor(0, 242, 255));

    buff_X.clear(); buff_Y.clear();
    for(int i = 0; i < result1.length(); i++)
    {
        buff_Y.push_back(mas[result9[i]]);
        buff_X.push_back(arrayFomatted[result9[i]]);
    }
    chartControl.AddGraphic(buff_X, buff_Y, "lsPoint", QColor(102, 0, 255));
}
