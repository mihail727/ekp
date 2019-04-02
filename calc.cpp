#include "calc.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

Calc::Calc(QObject *parent) : QObject(parent)
{
}

void Calc::doCalc()
{
    int i = 0;
    emit send(i);
}
