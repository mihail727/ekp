#ifndef METHODFACILITY_H
#define METHODFACILITY_H

#include <QObject>
#include <Methods/method.h>
#include <mainwindow.h>

class MethodFacility : public QObject
{
    Q_OBJECT
public:
    Method* Create(MainWindow::eMethod &methodText);

signals:

};

#endif // METHODFACILITY_H
