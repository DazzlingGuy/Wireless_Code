#include <QtCore/QCoreApplication>

#include "ServiceDataThread.h"
#include "ServicePortThread.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ServiceDataThread *serviceThread = new ServiceDataThread;
    serviceThread->start();

    return a.exec();
}