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
#include <QString>
#include <QMutex>
#include <QObject>
#include <QDebug>
#include <QByteArray>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "GABPNeuralNetworks.h"
#include "FuzzyReasoning.h"

enum PortValueType {
    Temperature = 0,
    COGas       = 1,
    CGGas       = 2
};

class ServicePortThread : public QThread
{
    Q_OBJECT
public:
    ServicePortThread();
    ~ServicePortThread();

    void run();

    void setBPNeuralNetworks(BPNeuralNetworks *network);
    static double normalization(double value, PortValueType type);
    static sample getPredictValue(QString portValue);
    samples getPredictValueList();

private slots:
	void readPortValue();

private:
    QSerialPort *m_pPort;
    /* No use */
    QString m_sSerialName;
    QString m_sSerialDescription;

    int m_nCount;
    clock_t m_ctTime;
    clock_t m_ctStart;
    clock_t m_ctEnd;

    bool m_bIsReadyPredict;
    bool m_bHasStartTime;
    bool m_bHasEndTime;

    QMutex *m_pMutex;

    sample m_sPredictValue;
    samples m_oPredictValueList;

    BPNeuralNetworks *m_pBPNeuralNetworks;
};

#endif //_DATATHREAD_H_