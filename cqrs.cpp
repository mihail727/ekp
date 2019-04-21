#include "cqrs.h"
#include <QVector>


QVector <double> masspoint1(1000);

void low (const QVector<double> &masspoint3, int firstCount, int secondCount) // фильтр нижних частот
        {
            int n = secondCount-firstCount;   
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
            
            int n = secondCount-firstCount;
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
            int n = secondCount-firstCount;
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
            int n = secondCount-firstCount;
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
            int n = secondCount-firstCount;
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
      

        void cQRS::doCalc(const QVector<double> &mas, int firstCount, int secondCount)
        {
            QVector<double> result(1000);

            low(mas, firstCount, secondCount );
            high(masspoint1, firstCount, secondCount );
            differentiation(masspoint1, firstCount, secondCount );
            square(masspoint1, firstCount, secondCount );
            result = integration(masspoint1, firstCount, secondCount );

            emit sendQRSValues(result);
            emit finished();
        }
