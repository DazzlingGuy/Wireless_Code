/*
* @file     ServicePortThread.cpp
* @brief    ServicePortThread
* @author   Ren SiLin
* @date     2018-04-15
* @modify   2018-04-15
* @remarks
* @Copyright (c) 2018-2019 rensl Corporation
*/

#include "ServiceUartThread.h"
#include "UartCollector.h"

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
				m_pCollector->setMutex(m_pMutex);
				m_pCollector->setServiceStatus(m_bIsServiceRun);
                cout << "Congratulation, service is running!" << endl;
            }
			samples predictValueList = m_pCollector->getPredictValueList();
            if (m_pCollector->getPredictStatus() && 0 != predictValueList.size())
            {
                QMutexLocker locker(m_pMutex);
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

                    double timeRate = m_pCollector->getTime() / 10.0;
                    if (timeRate > 1.0)
                    {
                        timeRate = 1.0;
                    }

                    FuzzyRule fuzzySingleRule;
                    FuzzyReasoning *fuzzyReasoning  = new FuzzyReasoning;   //Preparation of fuzzy inference rules
                    fuzzySingleRule.isFireRate      = fuzzyReasoning->normalMembership(predictValueList[i].outputValue[2]);
                    fuzzySingleRule.likeFireRate    = fuzzyReasoning->normalMembership(predictValueList[i].outputValue[1]);
                    fuzzySingleRule.time            = fuzzyReasoning->normalMembership(timeRate);
                    /* Change the time's PM to PB */
                    if (fuzzySingleRule.time == FUZZY_PM)
                    {
                        fuzzySingleRule.time = FUZZY_PB;
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