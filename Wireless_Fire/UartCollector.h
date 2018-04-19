#pragma once
/*
* @file     UartCollector.h
* @brief    UartCollector
* @author   Ren SiLin
* @date     2018-04-18
* @remarks
* @Copyright (c) 2018-2019 rensl Corporation
*/

#include <QObject>
#include <QMutex>
#include <QDebug>
#include <QByteArray>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "GABPNeuralNetworks.h"

/* Type of data collected */
enum UartValueType {
	Temperature = 0,
	COGas = 1,          
	CGGas = 2
};

class UartCollector : public QObject
{
	Q_OBJECT
public:
	UartCollector();
	~UartCollector();
public:
	bool isValidValue(QString portValue);   //Check the value valid
	double normalization(double value, UartValueType type); //value normalization

    /* Set corresponding flags value */
	bool getStartTimeStatus();
	void setStartTimeStatus(bool status);
	bool getEndTimeStatus();
	void setEndTimeStatus(bool status);
	bool getPredictStatus();
	void setPredictStatus(bool status);
	void setServiceStatus(bool status);
	void setCount(int count);
	void setMutex(QMutex *mutex);
	
	clock_t getTime();  //Get the last time

	sample getPredictValue(QString portValue);
    samples getPredictValueList();
	void clearPredictValueList();

private slots:
	void readPortValue();

private:
	int m_nCount;

	QSerialPort * m_pPort;

	/* No use */
	QString m_sSerialName;
	QString m_sSerialDescription;

	clock_t m_ctTime;
	clock_t m_ctStart;
	clock_t m_ctEnd;

    bool m_bServiceRun;
	bool m_bIsReadyPredict;
	bool m_bHasStartTime;
	bool m_bHasEndTime;

	QMutex *m_pMutex;

	sample m_sPredictValue;
	samples m_oPredictValueList;
};

