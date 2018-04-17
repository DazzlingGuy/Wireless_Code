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
    m_pMutex(new QMutex)
{
    m_oPredictValueList = getPredictValueList();
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
    //TODO:
}

void ServicePortThread::readPortValue()
{
    QString portValue(m_pPort->readAll());
    m_oPredictValueList = getPredictValueList(portValue);

    //The prescribed form is TM25.00|CO25.00|CG25.00#.
    //Then do the split.
}

samples ServicePortThread::getPredictValueList(QString winComValue) //TODO:
{
    QString predictStr(winComValue);
    QStringList predictStrList = predictStr.split("|");

    samples a;
    return a;
}

samples ServicePortThread::getPredictValueList()
{
    //Test samples writing
    doubles testInput[BP_TESE_NUMBER];
    doubles testOutput[BP_TESE_NUMBER];

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
void ServicePortThread::run()
{
    while(1)
    {
        if (m_pBPNeuralNetworks)
        {
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
                FuzzyReasoning *fuzzyReasoning  = new FuzzyReasoning;   //Preparation of fuzzy inference rules
                fuzzySingleRule.isFireRate      = fuzzyReasoning->normalMembership(m_oPredictValueList[i].outputValue[2]);
                fuzzySingleRule.likeFireRate    = fuzzyReasoning->normalMembership(m_oPredictValueList[i].outputValue[1]);
                fuzzySingleRule.time            = fuzzyReasoning->normalMembership(time);
                /* Change the time's PM to PB */
                if (fuzzySingleRule.time == FUZZY_PM)
                {
                    fuzzySingleRule.time = FUZZY_PB;
                }

                fuzzyReasoning->finalDecision(fuzzySingleRule);    //Final decision is get the fuzzy rules' final status to current final status.
                cout << "FinalStatus = " << fuzzySingleRule.finalStatus << endl;
            }

            break;  //TODO: Cycle to obtain serial data
        }
        else
        {
            cout << "Waiting for service begin !" << endl;
            sleep(3);
        }
    }
}