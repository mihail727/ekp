#include "calc.h"

void Calc::doCalc(QString fileName, int selectedLead, int firstCount, int secondCount)
{
    QVector<double> dataArray;
    QFile dataFile(fileName);
    dataArray.clear();
    minValueOfDataArray = 0;
    maxValueOfDataArray = 0;
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
/*ПРОВЕРКА ДОСТУПНОСТИ ФАЙЛА И ОПРЕДЕЛЕНИЕ МАКС СТРОК В НЕМ*/
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
    dataFile.setFileName(fileName);
    if ( (fileName == "") || (!dataFile.exists()) ||
            (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text)) ) {

        QString st("Ошибка чтения файла");
        emit sendError(st);
        return;
    }
    double maxValueOfCount = 0;
    while(!dataFile.atEnd()){
        QString buff = dataFile.readLine();
        maxValueOfCount++;
    };
    dataFile.close();
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
    if (secondCount>maxValueOfCount) {
        emit sendError("Введённные значения отсчета больше допустимых");
        return;
    }
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
            /*ЗАПОЛНЕНИЕ МАССИВА С ДАННЫМИ*/
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
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
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
            /*ОПРЕДЕЛЕНИЕ ГРАНИЦ ДЛЯ ГРАФИКА
                        * <<==>>
                * НАХОЖДЕНИЕ МАКС И МИН*/
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
    for(int i=0; i<dataArray.size()-1; i++) {
        if(minValueOfDataArray>dataArray[i])
            minValueOfDataArray = dataArray[i];
        if(maxValueOfDataArray<dataArray[i])
            maxValueOfDataArray = dataArray[i];
    }
    emit drawGraphic(dataArray, minValueOfDataArray,
                     maxValueOfDataArray);
}

Calc::~Calc()
{

}
