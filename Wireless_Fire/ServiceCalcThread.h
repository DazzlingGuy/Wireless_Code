#pragma once
/*
* @file     ServiceThread.h
* @brief    ServiceThread
* @author   Ren SiLin
* @date     2018-04-15
* @modify   2018-04-15
* @remarks
* @Copyright (c) 2018-2019 rensl Corporation
*/

#ifndef _SERVICETHREAD_H_
#define _SERVICETHREAD_H_

#include <QThread>
#include "GABPNeuralNetworks.h"
#include "UartCollector.h"
#include "FuzzyReasoning.h"

class ServiceCalcThread : public QThread
{
public:
    ServiceCalcThread();
    ~ServiceCalcThread();

    void run(); 

	void setCollector(UartCollector *);

    samples getLearnValueList();

private:
    samples m_oLearnValueList;

	UartCollector *m_pCollector;
};


#endif  //_SERVICETHREAD_H_