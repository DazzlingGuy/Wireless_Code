/*
* @file     ServiceUartThread.cpp
* @brief    ServiceUartThread
* @author   Ren SiLin
* @date     2018-04-15
* @remarks
* @Copyright (c) 2018-2019 rensl Corporation
*/

#include "ServiceUartThread.h"

using namespace std;

ServiceUartThread::ServiceUartThread() : 
    m_pBPNeuralNetworks(nullptr),
    m_pMutex(new QMutex),
    m_bIsServiceRun(false)
{
    QObject::connect(this, &QThread::finished, this, &QObject::deleteLater);
}

ServiceUartThread::~ServiceUartThread()
{
    //TODO: Destruct
}

void ServiceUartThread::setCollector(UartCollector *collector)
{
	m_pCollector = collector;
}

void ServiceUartThread::setBPNeuralNetworks(BPNeuralNetworks *networks)
{
    m_pBPNeuralNetworks = networks;
}

void ServiceUartThread::run()
{
    cout << "Waiting for service begin!" << endl;
    while(1)
    {
        if (m_pBPNeuralNetworks)
        {
            if (!m_bIsServiceRun)
			{
				m_bIsServiceRun = true;

                /* Set the mutex & bool value for the serial port collector */
				m_pCollector->setMutex(m_pMutex);
				m_pCollector->setServiceStatus(m_bIsServiceRun);

                cout << "Congratulation, service is running!" << endl;
            }

#ifdef _TEST
            emit m_pCollector->testSignal();
#endif //_TEST

            if (m_pCollector->getPredictStatus())
            {
                QMutexLocker locker(m_pMutex);
                samples predictValueList = m_pCollector->getPredictValueList();
                m_pBPNeuralNetworks->predict(predictValueList);
				for (int i = 0; i < predictValueList.size(); i++)
                {
                    for (int j = 0; j < predictValueList[i].inputValue.size(); j++)
                    {
                        cout << predictValueList[i].inputValue[j] << "\t";
                    }

                    cout << "prediction :";

                    for (int j = 0; j < predictValueList[i].outputValue.size(); j++)
                    {
                        cout << predictValueList[i].outputValue[j] << "\t";
                    }
#ifndef _TEST
                    double timeRate = m_pCollector->getTime() / 10.0;
                    if (timeRate > 1.0)
                    {
                        timeRate = 1.0;
                    }
#else
                    double timeRate = 8 / 10.0;
#endif  //_TEST

                    FuzzyRule fuzzySingleRule;
                    FuzzyReasoning *fuzzyReasoning  = new FuzzyReasoning;
                    fuzzySingleRule.isFireRate      = fuzzyReasoning->normalMembership(predictValueList[i].outputValue[2]);
                    fuzzySingleRule.likeFireRate    = fuzzyReasoning->normalMembership(predictValueList[i].outputValue[1]);
                    fuzzySingleRule.time            = fuzzyReasoning->normalMembership(timeRate);
                    if (fuzzySingleRule.time == FUZZY_PM)
                    {
                        fuzzySingleRule.time = FUZZY_PB;    //Change the time's PM to PB
                    }
                    fuzzyReasoning->finalDecision(fuzzySingleRule);    //Final decision is get the fuzzy rules' final status to current final status.
                    cout << "FinalStatus = " << fuzzySingleRule.finalStatus << endl;
                }

				m_pCollector->clearPredictValueList();
				m_pCollector->setPredictStatus(false);
				m_pCollector->setEndTimeStatus(false);
				m_pCollector->setStartTimeStatus(false);
				m_pCollector->setCount(0);
            }
        }
    }
}