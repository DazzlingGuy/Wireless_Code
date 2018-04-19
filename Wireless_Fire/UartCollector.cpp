/*
* @file     UartCollector.cpp
* @brief    UartCollector
* @author   Ren SiLin
* @date     2018-04-18
* @remarks
* @Copyright (c) 2018-2019 rensl Corporation
*/

#include "UartCollector.h"

#define UART_OBTAIN_COUNT	 10
#define ALARM_THRESHOLD		 0.5

static const QString g_sPortCom1 = "COM1";
static const QString g_sPortCom2 = "COM2";
static const QString g_sPortCom3 = "COM3";
static const QString g_sPortCom4 = "COM4";

static const QString g_sCoordinatorPortDescription	= "USB-SERIAL CH341A";
static const QString g_sTerminalPortDescription		= "USB Serial Port";

UartCollector::UartCollector() :
	m_bIsReadyPredict(false),
	m_bHasStartTime(false),
	m_bHasEndTime(false),
	m_bServiceRun(false),
	m_pPort(nullptr),
	m_nCount(0),
	m_ctTime(0),
	m_ctStart(0),
	m_ctEnd(0)
{ 
    /* init the m_sPredictValue */
	m_sPredictValue.inputValue.clear();
	m_sPredictValue.outputValue.clear();

    /* According to the description, initialize the serial port information */
	QSerialPortInfo *serialInfo = new QSerialPortInfo();
	QList<QSerialPortInfo> serialList = serialInfo->availablePorts();
	foreach(QSerialPortInfo winPort, serialList)
	{
		if (g_sTerminalPortDescription == winPort.description() || g_sCoordinatorPortDescription == winPort.description())
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

		QObject::connect(m_pPort, SIGNAL(readyRead()), this, SLOT(readPortValue()));
	}
}

UartCollector::~UartCollector()
{
}

void UartCollector::readPortValue()
{
	if (!m_bIsReadyPredict && m_bServiceRun)
	{
		QString portValue(m_pPort->readAll());
		if (isValidValue(portValue))
		{
			if (UART_OBTAIN_COUNT > m_nCount)
			{
				sample temp = getPredictValue(portValue);
				if (!m_bHasStartTime)
				{
					if (ALARM_THRESHOLD < temp.inputValue.at(0) || ALARM_THRESHOLD < temp.inputValue.at(1) || ALARM_THRESHOLD < temp.inputValue.at(2))
					{
						m_ctStart = clock();
						m_bHasStartTime = true;
					}
				}
				if (ALARM_THRESHOLD > temp.inputValue.at(0) && ALARM_THRESHOLD > temp.inputValue.at(1) && ALARM_THRESHOLD > temp.inputValue.at(2))
				{
					m_ctEnd = clock();
					m_bHasEndTime = true;
				}
				if (0 == m_sPredictValue.inputValue.size())
				{
					m_sPredictValue.inputValue.push_back(temp.inputValue[0]);
					m_sPredictValue.inputValue.push_back(temp.inputValue[1]);
					m_sPredictValue.inputValue.push_back(temp.inputValue[2]);
				}
				else
				{
					m_sPredictValue.inputValue[0] = m_sPredictValue.inputValue.at(0) + temp.inputValue.at(0);
					m_sPredictValue.inputValue[1] = m_sPredictValue.inputValue.at(1) + temp.inputValue.at(1);
					m_sPredictValue.inputValue[2] = m_sPredictValue.inputValue.at(2) + temp.inputValue.at(2);
				}
			}
			else
			{
				if (!m_bHasStartTime)
				{
					m_ctStart = clock();
				}
				if (!m_bHasEndTime || m_ctEnd < m_ctStart)
				{
					m_ctEnd = clock();
				}

				QMutexLocker locker(m_pMutex);
                m_sPredictValue.inputValue[0] = m_sPredictValue.inputValue.at(0) / UART_OBTAIN_COUNT;
                m_sPredictValue.inputValue[1] = m_sPredictValue.inputValue.at(1) / UART_OBTAIN_COUNT;
                m_sPredictValue.inputValue[2] = m_sPredictValue.inputValue.at(2) / UART_OBTAIN_COUNT;
				m_oPredictValueList.push_back(m_sPredictValue);

				m_ctTime = m_ctEnd - m_ctStart;

				m_bIsReadyPredict = true;

				m_sPredictValue.inputValue.clear();
				m_sPredictValue.outputValue.clear();
			}
			m_nCount++;
		}
	}
}

sample UartCollector::getPredictValue(QString portValue)
{
	QString predictStr(portValue);
	QStringList predictStrList = predictStr.split("|");

	sample predictValue;

    /* The split data is normalized into the sample */
	foreach(QString str, predictStrList)
	{
		if (str.contains(QString("TM")))
		{
			double value = normalization(str.replace(QString("TM"), QString("")).trimmed().toDouble(), UartValueType::Temperature);
			predictValue.inputValue.insert(predictValue.inputValue.begin(), value);
		}
		if (str.contains(QString("CO")))
		{
			double value = normalization(str.replace(QString("CO"), QString("")).trimmed().toDouble(), UartValueType::COGas);
			predictValue.inputValue.insert(predictValue.inputValue.begin() + 1, value);
		}
		if (str.contains(QString("CG")))
		{
			double value = normalization(str.replace(QString("CG"), QString("")).trimmed().toDouble(), UartValueType::CGGas);
			predictValue.inputValue.insert(predictValue.inputValue.begin() + 2, value);
		}
	}

	return predictValue;
}

bool UartCollector::isValidValue(QString portValue)
{
	bool bRes = true;

    /* Serial data requirements are not less than 17 bits, two reserved */
	if (17 > portValue.size() || 20 < portValue.size())
	{
		bRes = false;
	}

	return bRes;
}

double UartCollector::normalization(double value, UartValueType type)
{
	//Normalization, todo include optimize normalization rules
	double result(0.0);
	switch (type)
	{
	case Temperature:
		result = double((value - 20) / (125 - 20));
		break;
	case COGas:
		result = double((value - 45) / (127 - 45));
		break;
	case CGGas:
		result = double((value - 25) / (127 - 25));
		break;
	default:
		result = 0.0;
		break;
	}
	if (result < 0.0)
	{
		result = 0.0;
	}
	return result;
}

bool UartCollector::getStartTimeStatus()
{
	return m_bHasStartTime;
}

void UartCollector::setStartTimeStatus(bool status)
{
	m_bHasStartTime = status;;
}

bool UartCollector::getEndTimeStatus()
{
	return m_bHasEndTime;
}

void UartCollector::setEndTimeStatus(bool status)
{
	m_bHasEndTime = status;;
}

bool UartCollector::getPredictStatus()
{
	return m_bIsReadyPredict;
}

void UartCollector::setPredictStatus(bool status)
{
	m_bIsReadyPredict = status;
}

void UartCollector::setServiceStatus(bool status)
{
	m_bServiceRun = status;
}

clock_t UartCollector::getTime()
{
	return m_ctTime;
}

samples UartCollector::getPredictValueList()
{
	return m_oPredictValueList;
}

void UartCollector::clearPredictValueList()
{
	m_oPredictValueList.clear();
}

void UartCollector::setCount(int count)
{
	m_nCount = count;
}

void UartCollector::setMutex(QMutex *mutex)
{
	m_pMutex = mutex;
}