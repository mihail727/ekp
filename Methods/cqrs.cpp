#include "cqrs.h"
#include <QVector>
#include <iostream>
#include <QDebug>


static QVector <double> masspoint1(1000);

void low (const QVector<double> &masspoint3, int firstCount, int secondCount) // фильтр нижних частот
        {
            int n = secondCount;
            QVector <double> filter1(n);
            QVector <double> filter2(n);

            for (int i = firstCount; i < secondCount; i++)
            {
               filter1[i] = masspoint3[i];
            }

            for (int i = firstCount+12; i < secondCount; i++)
            {
                filter2[i] =(2 * filter2[i - 1] - filter2[i - 2] + (filter1[i] - 2 * filter1[i - 6] + filter1[i - 12]) / 32);
            }           
            masspoint1 = filter2;
           
        }

        void high(const QVector<double> &masspoint3, int firstCount, int secondCount)// фильтр верхних частот
        {
            
            int n = secondCount;
            QVector <double> filter1(n);
            QVector <double> filter2(n);

                
                 for (int i = firstCount; i < secondCount; i++)
                {
                    filter1[i] = masspoint3[i];
                }


                for (int i = firstCount+32; i < secondCount; i++)
                {
                    filter2[i]= (filter2[i - 1] - filter1[i] / 32 + filter1[i - 16] - filter1[i - 17] + filter1[i - 32] / 32);
                }
                

                masspoint1 = filter2;
            }

        void differentiation(const QVector<double> &masspoint3, int firstCount, int secondCount)//функция дифференцирования
        {
            int n = secondCount;
            QVector <double> filter1(n);
            QVector <double> filter2(n);


            for (int i = firstCount; i < secondCount; i++)
            {
                filter1[i] = masspoint3[i];
            }


            for (int i = firstCount+4; i < secondCount; i++)
            {
                filter2[i] = (2*filter1[i] + filter1[i-1] - filter1[i-3] - 2* filter1[i-4])/8;
            }

            masspoint1 = filter2;
        }

        void square(const QVector<double> &masspoint3, int firstCount, int secondCount)//функция возведения в квадрат
        {
            int n = secondCount;
            QVector <double> filter1(n);

            for (int i = firstCount; i < secondCount; i++)
            {
                filter1[i] = masspoint3[i] * masspoint3[i];
            }
        
            masspoint1 = filter1;
        }

        QVector<double> integration(const QVector<double> &masspoint3, int firstCount, int secondCount)// интегрирование
        {
            int N=100;
         //   N = Convert.ToInt32(comboBox1.Text); измени сам. с формы должна "ширина окна" передаваться
            int n = secondCount;
            QVector <double> filter1(n);
            QVector <double> filter2(n);


            for (int i = firstCount; i < secondCount; i++)
            {
                filter1[i] = masspoint3[i];
            }

            for (int i = firstCount; i < secondCount; i++)
            {
                filter2[i] = 0;
            }
            

            for (int i = firstCount+N; i < secondCount; i++)
            {
                double summa = 0;
                for (int j = N; j >= firstCount;  j--)
                    summa = summa + filter1[i-(j)];
                filter2[i] = summa / N;
            }
           
           
            masspoint1 = filter2;
            return masspoint1;
        }

        QVector<double> topsQRS(const QVector<double> &mas1, const QVector<double> &mas)
        {
            double max1 = 0;
            for (int i = 100; i < mas.size()-1; i++)
            {
                if (max1 < mas[i])
                    max1 = mas[i];
            }
            double max = 0;
            int n= mas.size();
            QVector<double> res(n);
            for (int i = 0; i< n; i ++)
            {
                res[i] = 0;
            }

            for (int i=90; i < n; i++)
            {

                while ((mas[i] >= max1/2) && (i < n-1))
                {
                    if (mas1[max] < mas1[i-90])
                        max = i-90;
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

                 min = mas[j];
             for (int i=mas[j] - 100; i < mas[j]; i++)
             {

                 while ((i < mas[j]))
                 {
                     if (mas1[min] > mas1[i])
                         min = i;
                     i++;

                 }
                 if (res.indexOf(min)==-1)
                res[j]= min;
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

                 min = mas[j];
                 for (int i=mas[j] + 100; i > mas[j]; i--)
                 {

                     while ((i > mas[j]))
                     {
                         if (mas1[min] > mas1[i])
                             min = i;
                         i--;
                     }
                     if (res.indexOf(min)==-1)
                         res[j]= min;
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

        void cQRS::doCalc(const QVector<double> &mas, int firstCount, int secondCount)
        {
            QVector<double> result(secondCount);
            QVector<double> result1(secondCount);
            QVector<double> result2(secondCount);
            QVector<double> result3(secondCount);
            low(mas, firstCount, secondCount );
            high(masspoint1, firstCount, secondCount );
            differentiation(masspoint1, firstCount, secondCount );
            square(masspoint1, firstCount, secondCount );
            result = integration(masspoint1, firstCount, secondCount );
            result1 = topsQRS(mas, result);
            result2 = topsQ(mas, result1);
            result3 = topsS(mas, result1);
            emit sendQRSValues (result, result1, mas, result2, result3);
            emit finished();
        }
