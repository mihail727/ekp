#include "calc.h"

void Calc::doCalc()
{    
    QFile dataFile(fileName);

    if ( (fileName == "") || (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text)) ) {
        QString st("Ошибка чтения файла");

        emit sendError(st);
        emit killThread();
        return;
    }

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
    }

    /* Сделать проверку ввода
        Заполнение массива*/

    dataFile.close();
    emit send(chrValue);
    emit killThread();
}
