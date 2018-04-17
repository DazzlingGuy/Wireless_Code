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
#include <QObject>
#include <QDebug>
#include <QByteArray>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "GABPNeuralNetworks.h"
#include "FuzzyReasoning.h"

class ServicePortThread : public QThread
{
    Q_OBJECT
public:
    ServicePortThread();
    ~ServicePortThread();

    void run();

    void setBPNeuralNetworks(BPNeuralNetworks *network);
    samples getPredictValueList();
    samples getPredictValueList(QString winComValue);

private slots:
	void readPortValue();

private:
    QSerialPort *m_pPort;

    QString m_sSerialName;
    QString m_sSerialDescription;

    bool m_bIsReadyPredict;
    QMutex *m_pMutex;

    samples m_oPredictValueList;
    BPNeuralNetworks *m_pBPNeuralNetworks;
};

#endif //_DATATHREAD_H_