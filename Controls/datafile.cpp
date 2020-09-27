#include "datafile.h"
QString DataFile::FileName = "";

DataFile::DataFile()
{

}

void DataFile::GenerateArrayData()
{
    if(FileName != previosFileName)
    {
        QString text = "";
        QFile file(FileName);

        if(FileName == "" || !file.exists())
        {
            throw QException();
            return;
        }

        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream stream(&file);
            while (!stream.atEnd()){
                text.append(stream.readLine());
            }
        }
        file.close();

        text.replace(QString(";"), QString(" "));
        text.replace(QString(","), QString(""));

        QStringList arrText = text.split(" ", Qt::SkipEmptyParts);

        QVector<int> arrayBuffData;
        for(int i = 0; i < arrText.length(); i++)
        {
            arrayBuffData.push_back(arrText[i].toInt());
        }

        while(arrayBuffData.length() % 12 != 0)
        {
            arrayBuffData.push_back(0);
        }

        ArrayData.resize(12);
        for(int i = 0; i < 12; i++)
            ArrayData[i].resize(arrayBuffData.length() / 12);

        int k = 0;
        for(int i = 0; i < 12; i++)
        {
            for(int j = i; j < arrayBuffData.length(); j+=12)
            {
                ArrayData[i][k] = arrayBuffData[j];
                k++;
            }
            k = 0;
        }

        previosFileName = FileName;

        MaxArrayDataSize = arrayBuffData.length() / 12;
    }
}

void DataFile::OpenFileDlg()
{
    QFileDialog fileDialog;
    fileDialog.setNameFilter(tr("TextFile (*.txt)"));
    if(fileDialog.exec())
        FileName = fileDialog.selectedFiles()[0];
}

QVector<double> DataFile::GenerateCurrentArray(int SamplesCount, int CurrentAbduction)
{
    QVector<double> mas(0);
    for(int i = 0; i < SamplesCount && i < ArrayData[CurrentAbduction].length(); i++)
    {
        mas.push_back(ArrayData[CurrentAbduction][i]);
    }
    return mas;
}
