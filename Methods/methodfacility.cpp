#include "methodfacility.h"
#include "cqrs.h"
#include "emd.h"

Method* MethodFacility::Create(MainWindow::eMethod& eMethod)
{
    Method* method = nullptr;
    if(eMethod == MainWindow::eMethod::ekp)
        method = new cQRS();
    else if (eMethod == MainWindow::eMethod::emd)
        method = new cEMD();

    return method;
}
