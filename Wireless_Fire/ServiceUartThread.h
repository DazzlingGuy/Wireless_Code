#pragma once
/*
* @file     DataThread.h
* @brief    DataThread
* @author   Ren SiLin
* @date     2018-04-15
* @modify   2018-04-15
* @remarks
* @Copyright (c) 2018-2019 rensl Corporation
*/

#ifndef _DATATHREAD_H_
#define _DATATHREAD_H_

#include <QThread>
#include <QMutex>
#include "GABPNeuralNetworks.h"
#include "UartCollector.h"
#include "FuzzyReasoning.h"

class ServiceUartThread : public QThread
{
    Q_OBJECT
public:
    ServiceUartThread();
    ~ServiceUartThread();

    void run();

	void setCollector(UartCollector *);

    void setBPNeuralNetworks(BPNeuralNetworks *network);

private:
    bool m_bIsServiceRun;

    QMutex *m_pMutex;

	BPNeuralNetworks *m_pBPNeuralNetworks;

	UartCollector *m_pCollector;
};

#endif //_DATATHREAD_H_