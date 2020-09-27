#ifndef DATAFILE_H
#define DATAFILE_H

#include <QObject>
#include <QFileDialog>
#include <QtDebug>
#include <QException>

class DataFile : public QObject
{
    Q_OBJECT
public:
    DataFile();
    int MaxArrayDataSize = 0;
    static QString FileName;
    QString previosFileName;
    QVector<QVector<int>> ArrayData; // двемерный массив данных из текста ( i - отведение, j - значение )

    void GenerateArrayData(); // формирует из текста двумерный массив на 12 отведений
    static void OpenFileDlg();
    QVector<double> GenerateCurrentArray(int SamplesCount, int CurrentAbduction); // формирует одномерный массив для текущего отведения

private:

signals:

};

#endif // DATAFILE_H
