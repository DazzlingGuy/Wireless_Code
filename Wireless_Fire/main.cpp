#include <QtCore/QCoreApplication>
#include "ServiceCalcThread.h"
#include "UartCollector.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

	UartCollector *collector = new UartCollector;

	ServiceCalcThread *serviceThread = new ServiceCalcThread;
	serviceThread->setCollector(collector);
	serviceThread->start();

    return a.exec();
}