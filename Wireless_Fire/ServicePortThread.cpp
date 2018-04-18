/*
* @file     ServicePortThread.cpp
* @brief    ServicePortThread
* @author   Ren SiLin
* @date     2018-04-15
* @modify   2018-04-15
* @remarks
* @Copyright (c) 2018-2019 rensl Corporation
*/

#include "ServicePortThread.h"

#define PORT_OBTIN_COUNT 10

#define _TEST _DEBUG_TEST

static const QString g_sPortCom1         = "COM1";
static const QString g_sPortCom2         = "COM2";
static const QString g_sPortCom3         = "COM3";
static const QString g_sPortCom4         = "COM4";
static const QString g_sPortDescription  = "123abc";

using namespace std;

ServicePortThread::ServicePortThread() : 
    m_pBPNeuralNetworks(nullptr),
    m_bIsReadyPredict(false),
    m_pPort(nullptr),
    m_nCount(0),
    m_pMutex(new QMutex)
{
#ifdef _TEST
    m_oPredictValueList = getPredictValueList();
#endif
    m_sPredictValue.inputValue.clear();
    m_sPredictValue.outputValue.clear();
    QSerialPortInfo *serialInfo = new QSerialPortInfo();
    QList<QSerialPortInfo> serialList = serialInfo->availablePorts();

    foreach(QSerialPortInfo winPort, serialList)
    {
        if (g_sPortDescription == winPort.description())
        {
            m_sSerialDescription = winPort.description();
            m_sSerialName = winPort.portName();
        }
    }

    if (!m_sSerialDescription.isNull() && !m_sSerialName.isNull())
    {
        m_pPort = new QSerialPort(m_sSerialName);
        m_pPort->setBaudRate(QSerialPort::Baud115200);
        m_pPort->setDataBits(QSerialPort::Data8);
        m_pPort->setParity(QSerialPort::NoParity);
        m_pPort->setStopBits(QSerialPort::OneStop);
        m_pPort->setFlowControl(QSerialPort::NoFlowControl);

        if (!m_pPort->open(QIODevice::ReadOnly))
        {
            qDebug() << "Open Failed !";
        }

        QObject::connect(m_pPort, &QSerialPort::readyRead, this, &ServicePortThread::readPortValue);
    }

    QObject::connect(this, &QThread::finished, this, &QObject::deleteLater);
}

ServicePortThread::~ServicePortThread()
{
    //TODO: Destruct
}

void ServicePortThread::readPortValue()
{
    //The prescribed form is TM25.00|CO25.00|CG25.00.
    //Then do the split.
    if (!m_bIsReadyPredict)
    {
        QString portValue(m_pPort->readAll());
        if (PORT_OBTIN_COUNT > m_nCount)
        {
            sample temp = getPredictValue(portValue);
            m_sPredictValue.inputValue.at(0) += temp.inputValue.at(0);
            m_sPredictValue.inputValue.at(1) += temp.inputValue.at(1);
            m_sPredictValue.inputValue.at(2) += temp.inputValue.at(2);
        }
        else
        {
            QMutexLocker locker(m_pMutex);
            m_sPredictValue.inputValue.at(0) /= PORT_OBTIN_COUNT;
            m_sPredictValue.inputValue.at(1) /= PORT_OBTIN_COUNT;
            m_sPredictValue.inputValue.at(2) /= PORT_OBTIN_COUNT;
            m_oPredictValueList.push_back(m_sPredictValue);

            m_bIsReadyPredict = true;
            m_sPredictValue.inputValue.clear();
            m_sPredictValue.outputValue.clear();
        }

        m_nCount++;
    }
}

sample ServicePortThread::getPredictValue(QString portValue)
{
    QString predictStr(portValue);
    QStringList predictStrList = predictStr.split("|");

    sample predictValue;

    foreach(QString str, predictStrList)
    {
        if (str.contains(QString("TM")))
        {
            //TODO: Normalization
            double value = normalization(str.replace(QString("TM"), QString("")).trimmed().toDouble(), PortValueType::Temperature);
            predictValue.inputValue.insert(predictValue.inputValue.begin(), value);
        }
        if (str.contains(QString("CO")))
        {
            //TODO: Normalization
            double value = normalization(str.replace(QString("CO"), QString("")).trimmed().toDouble(), PortValueType::COGas);
            predictValue.inputValue.insert(predictValue.inputValue.begin() + 1, value);
        }
        if (str.contains(QString("CG")))
        {
            //TODO: Normalization
            double value = normalization(str.replace(QString("CG"), QString("")).trimmed().toDouble(), PortValueType::CGGas);
            predictValue.inputValue.insert(predictValue.inputValue.begin() + 2, value);
        }
    }

    return predictValue;
}

samples ServicePortThread::getPredictValueList()
{
    //Test samples writing
    doubles testInput[BP_TESE_NUMBER];

    testInput[0].push_back(0.5);    testInput[0].push_back(0.5);    testInput[0].push_back(0.5);
    testInput[1].push_back(0);      testInput[1].push_back(0.8);    testInput[1].push_back(0.6);
    testInput[2].push_back(0.55);   testInput[2].push_back(0.83);   testInput[2].push_back(0.75);
    testInput[3].push_back(0.375);  testInput[3].push_back(0.37);   testInput[3].push_back(0.45);
    testInput[4].push_back(1);      testInput[4].push_back(0.2);    testInput[4].push_back(0.9);

    samples testValueList;
    for (int i = 0; i < BP_TESE_NUMBER; i++)
    {
        sample testInAndOut;
        testInAndOut.inputValue = testInput[i];
        testValueList.push_back(testInAndOut);
    }

    return testValueList;
}

void ServicePortThread::setBPNeuralNetworks(BPNeuralNetworks *networks)
{
    m_pBPNeuralNetworks = networks;
}

double ServicePortThread::normalization(double value, PortValueType type)
{
    return 0.0;
}

void ServicePortThread::run()
{
    while(1)
    {
        if (m_pBPNeuralNetworks)
        {
#ifndef _TEST
            if (m_bIsReadyPredict && 0 != m_oPredictValueList.size())
            {
#endif
                QMutexLocker locker(m_pMutex);
                m_pBPNeuralNetworks->predict(m_oPredictValueList);
                for (int i = 0; i < m_oPredictValueList.size(); i++)
                {
                    for (int j = 0; j < m_oPredictValueList[i].inputValue.size(); j++)
                    {
                        cout << m_oPredictValueList[i].inputValue[j] << "\t";
                    }
                    cout << "prediction :";
                    for (int j = 0; j < m_oPredictValueList[i].outputValue.size(); j++)
                    {
                        cout << m_oPredictValueList[i].outputValue[j] << "\t";
                    }

                    double time = 8 / 10.0; //TODO: Change the file least time.

                    FuzzyRule fuzzySingleRule;
                    FuzzyReasoning *fuzzyReasoning = new FuzzyReasoning;   //Preparation of fuzzy inference rules
                    fuzzySingleRule.isFireRate = fuzzyReasoning->normalMembership(m_oPredictValueList[i].outputValue[2]);
                    fuzzySingleRule.likeFireRate = fuzzyReasoning->normalMembership(m_oPredictValueList[i].outputValue[1]);
                    fuzzySingleRule.time = fuzzyReasoning->normalMembership(time);
                    /* Change the time's PM to PB */
                    if (fuzzySingleRule.time == FUZZY_PM)
                    {
                        fuzzySingleRule.time = FUZZY_PB;
                    }

                    fuzzyReasoning->finalDecision(fuzzySingleRule);    //Final decision is get the fuzzy rules' final status to current final status.
                    cout << "FinalStatus = " << fuzzySingleRule.finalStatus << endl;
                }
#ifndef _TEST
                m_oPredictValueList.clear();
                m_bIsReadyPredict = false;
                m_nCount = 0;
            }
#else
                break;
#endif // _Test
        }
        else
        {
            cout << "Waiting for service begin !" << endl;
            sleep(3);
        }
    }
}