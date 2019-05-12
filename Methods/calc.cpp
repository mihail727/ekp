#include "calc.h"

void Calc::doCalc(QString &fileName, int &selectedLead,
                  int &firstCount, int &secondCount)
{
    QFile dataFile(fileName);
    dataArray.clear();
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
                /*ПРОВЕРКА ДОСТУПНОСТИ ФАЙЛА*/
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
    dataFile.setFileName(fileName);
    if ( (fileName == "") || (!dataFile.exists()) ||
            (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text)) ) {

        QString st("Ошибка чтения файла");
        emit sendError(st);
        return;
    }
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
            /*ЗАПОЛНЕНИЕ МАССИВА С ДАННЫМИ*/
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
    dataFile.close();

    dataFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString chrValue;

    for(int j=0; j<secondCount; j++) {
        QString dataText = dataFile.readLine();
        if (j<firstCount)
            continue;

        if (selectedLead != 1) {
            for (int i = 1; i<selectedLead; i++) {
                int chrIndex = dataText.indexOf(" ");
                dataText.remove(0,chrIndex+1);
            }
        }

        int i = 0;
        chrValue = "";

        while((dataText[i] != ",") && (dataText[i] != ";")) {
            chrValue = chrValue + dataText[i];
            i++;
        }

        double Value = chrValue.toDouble();
        dataArray.push_back(Value);
    }

    dataFile.close();

    emit drawGraphic(dataArray);
    emit finished(dataArray);
}
