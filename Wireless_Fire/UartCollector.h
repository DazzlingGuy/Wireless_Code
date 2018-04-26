#pragma once
/*
* @file     UartCollector.h
* @brief    UartCollector
* @author   Ren SiLin
* @date     2018-04-18
* @remarks
* @Copyright (c) 2018-2019 rensl Corporation
*/
#ifndef _UARTCOLLECTOR_H_
#define _UARTCOLLECTOR_H_

#include <QObject>
#include <QMutex>
#include <QDebug>
#include <QByteArray>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "GABPNeuralNetworks.h"

#define UART_OBTAIN_COUNT	 4
#define ALARM_THRESHOLD		 0.5

//#define _TEST __TEST

static const QString g_sPortCom1 = "COM1";
static const QString g_sPortCom2 = "COM2";
static const QString g_sPortCom3 = "COM3";
static const QString g_sPortCom4 = "COM4";

static const QString g_sCoordinatorPortDescription  = "USB-SERIAL CH341A";
static const QString g_sTerminalPortDescription     = "USB Serial Port";

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
	void readUartValue();

#ifdef _TEST
    void testUartValue();

signals:
    void testSignal();
#endif //_TEST

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

#endif  //_UARTCOLLECTOR_H_