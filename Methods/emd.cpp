#include "emd.h"
#include "QVector"

#define NZEROS 1
#define NPOLES 1
#define GAIN   1.726542528e+00F
#define OUR_GAIN 2.1F		// наш коэф. усиления фильтра

float xv[NZEROS + 1], yv[NPOLES + 1];

void CEMDeco(QVector<double> _x, QVector<double> &_CEMd)
 {
    //Преобразование Гильберта-Хаунга
    QVector<double> m;//Функции средних значений
    QVector<QVector<double>> Reside;//функция эмперических мод
    {
        //Нахождение эмперических мод
        m.push_back(_x[0]);
        for (int i = 1; i < _x.size() - 1; i++)
        {
            m.push_back((_x[i - 1] + _x[i + 1]) / 2);//Расчет среднего значения
        }
        m.push_back(_x[_x.size() - 1]);

        Reside.push_back(QVector<double>(0));
        for (int i = 0; i < _x.size(); i++)
        {
            Reside[0].push_back(_x[i] - m[i]);//Расчет моды функции
        }
        m[0] = Reside[0][0];
        for (int i = 1; i < Reside.size() - 1; i++)
        {
            m[i] = Reside[0][i - 1] - Reside[0][i + 1];
        }
        m[m.size() - 1] = Reside[0][Reside.size() - 1];

        for (int j = 1; j < 3; j++)
        {
            Reside.push_back(QVector<double>(j));
            for (int i = 0; i < _x.size()-1; i++)
            {
                Reside[j].push_back(Reside[j-1][i] - m[i]);//Расчет моды функции
            }

            if (j + 1 < 3)
            {
                m[0] = Reside[j][0];
                for (int i = 1; i < Reside.size() - 1; i++)
                {
                    m[i] = Reside[j][i - 1] - Reside[j][i + 1];
                }
                m[m.size() - 1] = Reside[j][Reside.size() - 1];
            }
        }
        m.clear();
        for (int i = 0; i < _x.size(); i++)
        {
            m.push_back(pow((Reside[0][i] + Reside[1][i] + Reside[2][i]), 2));
        }
        _CEMd = m;
        m.clear();
        Reside.clear();
    }
}

void CEMD(QVector<double> &AFileD)
 {
     QVector<double> C01;

     CEMDeco(AFileD, C01);

     AFileD = C01;
     C01.clear();
 }

void FNCH(QVector<double> &_FileCEMD)
 {
     QVector<double> Fnch;
     for (int i = 0; i < _FileCEMD.size(); i++)
     {
         xv[0] = xv[1];
         xv[1] = _FileCEMD[i] / GAIN;
         yv[0] = yv[1];
         yv[1] = (xv[1] - xv[0]) + (0.1583844403F * yv[0]);
         Fnch.push_back(((yv[1] * OUR_GAIN) + _FileCEMD[i]));
     }
     _FileCEMD.clear();
     _FileCEMD.reserve(Fnch.size());
     _FileCEMD = Fnch;
     Fnch.clear();
 }

void Max(QVector<double> &_FileCEMD)
{
     QVector<double> Max;
     for (int i = 1; i < _FileCEMD.size()-1; i++)
     {
         if ((_FileCEMD[i-1] < _FileCEMD[i] && _FileCEMD[i] > _FileCEMD[i+1]))
         {
             Max.push_back(_FileCEMD[i]);
         }
         else
         {
             Max.push_back(0);
         }
     }
     _FileCEMD.clear();
     _FileCEMD.reserve(Max.size());
     _FileCEMD = Max;
     Max.clear();
}

void Min(QVector<double> &_FileCEMD)
{
     QVector<double> Max;
     for (int i = 1; i > _FileCEMD.size()-1; i++)
     {
         if ((_FileCEMD[i-1] > _FileCEMD[i] && _FileCEMD[i] < _FileCEMD[i+1]))
         {
             Max.push_back(_FileCEMD[i]);
         }
         else
         {
             Max.push_back(0);
         }
     }
     _FileCEMD.clear();
     _FileCEMD.reserve(Max.size());
     _FileCEMD = Max;
     Max.clear();
}

void Alg(QVector<double> &_FileCEMD)
{
     double flag = 0;
     double CInterval = 600;
     double max = 0, interval = 0;
     auto it = _FileCEMD.begin(), it1 = _FileCEMD.begin();
     max = *it;
     it++;
     for (; it < _FileCEMD.end(); it++)
     {
         if (max < abs(*it))
         {
             max = abs(*it);
         }
     }
     for (it = _FileCEMD.begin(); it < _FileCEMD.end(); it++)
     {
         if (max*0.4 > abs(*it))
         {
             *it = 0;
         }
     }
     if (CInterval > _FileCEMD.size() - 1)
     {
         CInterval = _FileCEMD.size() - 1;
     }
     it = _FileCEMD.begin();
     max = 0;
     for (; it < _FileCEMD.end(); it++)
     {
         if (*it != 0)
         {
             if (flag == 0)
             {
                 it1 = it;
                 max = *it;
             }
             else
             {
                 if (max < abs(*it))
                 {
                     max = abs(*it);
                     *it1 = 0;
                     it1 = it;
                 }
                 else
                 {
                     *it = 0;
                 }
             }
             flag = 1;
         }
         if (flag == 1)
         {
             interval++;
             if (CInterval == interval)
             {
                flag = 0;
                max = 0;
                interval = 0;
             }
         }
     }
}



QVector<double> SearchPickP(QVector<double> &_FileCEMD, QVector<double> &_FileR)
{
    QVector<double> _PickP;
    int rad=200,indexj;
    double max = 0;
    for(int i=0; i<_FileCEMD.size() ;i++)
    {
        if(_FileR[i] != 0)
        {
            max=0;
            for(int j=i-rad;j<i-30;j++)
            {
                if(_FileCEMD[j]>max)
                {
                    max=_FileCEMD[j];
                    indexj = j;
                }
            }
            _PickP.push_back(0);
            _PickP[indexj]=max;
        }
        else
        {
            _PickP.push_back(0);
        }
    }
    return _PickP;
    _PickP.clear();
}

void SearchPickT(QVector<double> &_FileCEMD, QVector<double> &_FileR, QVector<double> &_PickS)
{
    int rad=400,ind=-1;
    QVector<int> indexj;
    QVector<double> max;
    for(int i=0; i<_FileCEMD.size() ;i++)
    {
        if(_FileR[i] != 0)
        {
            max.push_back(0);
            indexj.push_back(0);
            ind++;
            for(int j=i+rad;j>i+30;j--)
            {
                if(_FileCEMD[j]>max[ind])
                {
                    max[ind]=_FileCEMD[j];
                    indexj[ind] = j;
                }
            }
        }
        _PickS.push_back(0);
    }
    for(int i=0; i<indexj.size() ;i++)
    {
        _PickS[indexj[i]]=max[i];
    }
}

QVector<double> Proizvodnaya(const QVector<double> mas)
{
    int n = mas.size();
    QVector<double> res(n);
    for (int i = 40; i< n; i++)
    {
       res[i] = abs(mas[i]-mas[i-40]);
    }

    return res;
}

QVector<double> beginP(QVector<double> &diff, const QVector<double> mas)
{
    QVector<double> res;

    int ind=-1;
    QVector<int> indexj;
    QVector<double> max;
    for(int i=0; i<mas.size() ;i++)
    {
        if(mas[i] != 0)
        {
            max.push_back(diff[i]);
            indexj.push_back(0);
            ind++;
            for(int j=i;;j--)
            {
                if(diff[j]<max[ind]*0.1 )
                {
                    if(diff[j] != 0)
                    {
                        max[ind]=diff[j];
                        indexj[ind] = j;
                        break;
                    }
                }
            }
        }
        res.push_back(0);
    }

    for(int i=0; i<indexj.size() ;i++)
    {
        res[indexj[i]]=max[i];
    }

    return res;

}

QVector<double> endsP(QVector<double> &diff, const QVector<double> mas)
{
    QVector<double> res;

    int ind=-1;
    QVector<int> indexj;
    QVector<double> max;
    for(int i=0; i<mas.size() ;i++)
    {
        if(mas[i] != 0)
        {
            max.push_back(diff[i]);
            indexj.push_back(0);
            ind++;
            for(int j=i;;j++)
            {
                if(diff[j]<max[ind]*0.3 )
                {
                    if(diff[j] != 0)
                    {
                        max[ind]=diff[j];
                        indexj[ind] = j;
                        break;
                    }
                }
            }
        }
        res.push_back(0);
    }

    for(int i=0; i<indexj.size() ;i++)
    {
        res[indexj[i]]=max[i];
    }

    return res;
}

QVector<double> beginT(const QVector<double> &diff, const QVector<double> mas)
{
    QVector<double> res;

    int ind=-1;
    QVector<int> indexj;
    QVector<double> max;
    for(int i=0; i<mas.size() ;i++)
    {
        if(mas[i] != 0)
        {
            max.push_back(diff[i]);
            indexj.push_back(0);
            ind++;
            for(int j=i;;j--)
            {
                if(diff[j]<max[ind]*0.1 )
                {
                    if(diff[j] != 0)
                    {
                        max[ind]=diff[j];
                        indexj[ind] = j;
                        break;
                    }
                }
            }
        }
        res.push_back(0);
    }

    for(int i=0; i<indexj.size() ;i++)
    {
        res[indexj[i]]=max[i];
    }

    return res;

}

QVector<double> endsT(QVector<double> diff, const QVector<double> mas)
{
    QVector<double> res;

    int ind=-1,f=0;
    QVector<int> indexj;
    QVector<double> max;
    for(int i=0; i<mas.size() ;i++)
    {
        if(mas[i] != 0)
        {
            max.push_back(diff[i]);
            indexj.push_back(0);
            ind++;
            for(int j=i;;j++)
            {
                if(diff[j]<max[ind]*0.1 )
                {
                    if(diff[j] != 0)
                    {
                        max[ind]=diff[j];
                        indexj[ind] = j;
                        if(f==1)
                        {
                            break;
                        }
                        f=1;
                    }
                }
            }
        }
        res.push_back(0);
        f=0;
    }

    for(int i=0; i<indexj.size() ;i++)
    {
        res[indexj[i]]=max[i];
    }

    return res;
}

void cEMD::doCalc(const QVector<double> &mas, ChartControl &chartControl,
                  QVector<double> arrayFomatted)
{
    QVector<double> FileD = mas, PickQRS, PickP, PickT, Difer, BgPickP, NdPickP, BgPickT, NdPickT;
    QVector<double> FileY, FileX;
    CEMD(FileD);
    FNCH(FileD);
    Max(FileD);

    PickQRS = FileD;

    Alg(PickQRS);

    // Основной график
    chartControl.AddGraphic(arrayFomatted, mas, "lsLine");

    // R pick
    for(int i=10;i<PickQRS.size();i++)
    {
        if(PickQRS[i] != 0)
        {
            FileX.push_back(mas[i]);
            FileY.push_back(arrayFomatted[i]);
        }
    }
    chartControl.AddGraphic(FileY, FileX, "lsPoint", Qt::red, Qt::black);
    FileX.clear(); FileY.clear();

    PickP = SearchPickP(FileD, PickQRS);

    // P pick
    for(int i=0;i<PickP.size();i++)
    {
        if(PickP[i] != 0)
        {
            FileX.push_back(mas[i]);
            FileY.push_back(arrayFomatted[i]);
        }
    }
    chartControl.AddGraphic(FileY, FileX, "lsPoint", Qt::green, Qt::black);
    FileX.clear(); FileY.clear();

    SearchPickT(FileD, PickQRS, PickT);

    // T pick
    for(int i=0;i<PickT.size();i++)
    {
        if(PickT[i] != 0)
        {
            FileX.push_back(mas[i]);
            FileY.push_back(arrayFomatted[i]);
        }
    }
    chartControl.AddGraphic(FileY, FileX, "lsPoint", QColor(156, 156, 156), Qt::black);
    FileX.clear(); FileY.clear();

    Difer = Proizvodnaya(FileD);

    BgPickP = beginP(Difer, PickP);

    for(int i=0;i<BgPickP.size();i++)
    {
        if(BgPickP[i] != 0)
        {
            FileX.push_back(mas[i]);
            FileY.push_back(arrayFomatted[i]);
        }
    }
    chartControl.AddGraphic(FileY, FileX, "lsPoint", QColor(255, 123, 0), Qt::black);
    FileX.clear(); FileY.clear();

    NdPickP = endsP(Difer, PickP);

    for(int i=0;i<NdPickP.size();i++)
    {
        if(NdPickP[i] != 0)
        {
            FileX.push_back(mas[i]);
            FileY.push_back(arrayFomatted[i]);
        }
    }
    chartControl.AddGraphic(FileY, FileX, "lsPoint", Qt::blue, Qt::black);
    FileX.clear(); FileY.clear();

    BgPickT = beginT(Difer, PickT);

    for(int i=0;i<BgPickT.size();i++)
    {
        if(BgPickT[i] != 0)
        {
            FileX.push_back(mas[i]);
            FileY.push_back(arrayFomatted[i]);
        }
    }
    chartControl.AddGraphic(FileY, FileX, "lsPoint", QColor(64,55,0), Qt::black);
    FileX.clear(); FileY.clear();

    NdPickT = endsT(Difer, PickT);

    for(int i=0;i<NdPickT.size();i++)
    {
        if(NdPickT[i] != 0)
        {
            FileX.push_back(mas[i]);
            FileY.push_back(arrayFomatted[i]);
        }
    }
    chartControl.AddGraphic(FileY, FileX, "lsPoint", QColor(64,55,0), Qt::black);
    FileX.clear(); FileY.clear();
}
