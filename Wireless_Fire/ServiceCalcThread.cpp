#include "ServiceCalcThread.h"
/*
* @file     ServiceThread.cpp
* @brief    ServiceThread
* @author   Ren SiLin
* @date     2018-04-15
* @remarks
* @Copyright (c) 2018-2019 rensl Corporation
*/

#include "ServiceUartThread.h"

ServiceCalcThread::ServiceCalcThread()
{
    m_oLearnValueList = getLearnValueList();

    QObject::connect(this, &QThread::finished, this, &QObject::deleteLater);
}

ServiceCalcThread::~ServiceCalcThread()
{
    qDebug() << "Service calculator is finished, now begin read uart value";
}

void ServiceCalcThread::setCollector(UartCollector *collector)
{
	m_pCollector = collector;
}

samples ServiceCalcThread::getLearnValueList()
{
    //Learn samples writing
    doubles sampleInput[BP_TRAIN_NUMBER];
    doubles sampleOutput[BP_TRAIN_NUMBER];

	/* Thesis conclusion */
    sampleInput[0].push_back(1);        sampleInput[0].push_back(0.2);      sampleInput[0].push_back(0.9);
    sampleOutput[0].push_back(0);       sampleOutput[0].push_back(0.05);    sampleOutput[0].push_back(0.95);

    sampleInput[1].push_back(0.9);      sampleInput[1].push_back(0.23);     sampleInput[1].push_back(1);
    sampleOutput[1].push_back(0);       sampleOutput[1].push_back(0.05);    sampleOutput[1].push_back(0.95);

    sampleInput[2].push_back(1);        sampleInput[2].push_back(0.22);     sampleInput[2].push_back(0.85);
    sampleOutput[2].push_back(0.02);    sampleOutput[2].push_back(0.08);    sampleOutput[2].push_back(0.9);

    sampleInput[3].push_back(0.9);      sampleInput[3].push_back(0.22);     sampleInput[3].push_back(1);
    sampleOutput[3].push_back(0.02);    sampleOutput[3].push_back(0.08);    sampleOutput[3].push_back(0.9);

    sampleInput[4].push_back(0.875);    sampleInput[4].push_back(0.18);     sampleInput[4].push_back(0.35);
    sampleOutput[4].push_back(0.05);    sampleOutput[4].push_back(0.1);     sampleOutput[4].push_back(0.85);

    sampleInput[5].push_back(0.95);     sampleInput[5].push_back(0.2);      sampleInput[5].push_back(0.75);
    sampleOutput[5].push_back(0.03);    sampleOutput[5].push_back(0.1);     sampleOutput[5].push_back(0.85);

    sampleInput[6].push_back(0.9);      sampleInput[6].push_back(0.13);     sampleInput[6].push_back(0.2);
    sampleOutput[6].push_back(0.1);     sampleOutput[6].push_back(0.1);     sampleOutput[6].push_back(0.8);

    sampleInput[7].push_back(0.925);    sampleInput[7].push_back(0.18);     sampleInput[7].push_back(0.25);
    sampleOutput[7].push_back(0.1);     sampleOutput[7].push_back(0.1);     sampleOutput[7].push_back(0.8);

    sampleInput[8].push_back(0.6);      sampleInput[8].push_back(0.1);		sampleInput[8].push_back(0.5);	//modify
    sampleOutput[8].push_back(0.7);     sampleOutput[8].push_back(0.1);     sampleOutput[8].push_back(0.2);

    sampleInput[9].push_back(1);        sampleInput[9].push_back(0.1);		sampleInput[9].push_back(0.75);	//modify
    sampleOutput[9].push_back(0.7);     sampleOutput[9].push_back(0.1);     sampleOutput[9].push_back(0.2);

    sampleInput[10].push_back(0.45);    sampleInput[10].push_back(0.15);    sampleInput[10].push_back(0.5);
    sampleOutput[10].push_back(0.05);   sampleOutput[10].push_back(0.8);    sampleOutput[10].push_back(0.15);

    sampleInput[11].push_back(0.45);    sampleInput[11].push_back(0.15);    sampleInput[11].push_back(0.7);
    sampleOutput[11].push_back(0.05);   sampleOutput[11].push_back(0.85);   sampleOutput[11].push_back(0.1);

    sampleInput[12].push_back(0.45);    sampleInput[12].push_back(0.18);    sampleInput[12].push_back(0.75);
    sampleOutput[12].push_back(0.05);   sampleOutput[12].push_back(0.85);   sampleOutput[12].push_back(0.1);

    sampleInput[13].push_back(0.6);     sampleInput[13].push_back(0.7);     sampleInput[13].push_back(0.45);
    sampleOutput[13].push_back(0.8);    sampleOutput[13].push_back(0.1);    sampleOutput[13].push_back(0.1);

    sampleInput[14].push_back(1);       sampleInput[14].push_back(0.42);    sampleInput[14].push_back(0.7);
    sampleOutput[14].push_back(0.85);   sampleOutput[14].push_back(0.05);   sampleOutput[14].push_back(0.1);

    sampleInput[15].push_back(0.5);     sampleInput[15].push_back(0.25);    sampleInput[15].push_back(0.7);
    sampleOutput[15].push_back(0.02);   sampleOutput[15].push_back(0.9);    sampleOutput[15].push_back(0.08);

    sampleInput[16].push_back(0.525);   sampleInput[16].push_back(0.5);     sampleInput[16].push_back(0.75);
    sampleOutput[16].push_back(0.02);   sampleOutput[16].push_back(0.9);    sampleOutput[16].push_back(0.08);

    sampleInput[17].push_back(0.55);    sampleInput[17].push_back(0.83);    sampleInput[17].push_back(0.75);
    sampleOutput[17].push_back(0);      sampleOutput[17].push_back(0.95);   sampleOutput[17].push_back(0.05);

    sampleInput[18].push_back(0.375);   sampleInput[18].push_back(0.37);    sampleInput[18].push_back(0.45);
    sampleOutput[18].push_back(0.85);   sampleOutput[18].push_back(0.1);    sampleOutput[18].push_back(0.05);

    sampleInput[19].push_back(0.625);   sampleInput[19].push_back(0.2);     sampleInput[19].push_back(0.5);
	sampleOutput[19].push_back(0.85);   sampleOutput[19].push_back(0.1);    sampleOutput[19].push_back(0.05);

	/* Internet Nymph */
	sampleInput[20].push_back(0.45);    sampleInput[20].push_back(0.15);    sampleInput[20].push_back(0.5);
	sampleOutput[20].push_back(0.05);   sampleOutput[20].push_back(0.79);	sampleOutput[20].push_back(0.15);

	sampleInput[21].push_back(0.45);	sampleInput[21].push_back(0.15);	sampleInput[21].push_back(0.65);
	sampleOutput[21].push_back(0.05);   sampleOutput[21].push_back(0.81);	sampleOutput[21].push_back(0.12);

	sampleInput[22].push_back(0.45);	sampleInput[22].push_back(0.17);	sampleInput[22].push_back(0.75);
	sampleOutput[22].push_back(0.03);	sampleOutput[22].push_back(0.85);	sampleOutput[22].push_back(0.1);

	sampleInput[23].push_back(0.45);	sampleInput[23].push_back(0.18);	sampleInput[23].push_back(0.75);
	sampleOutput[23].push_back(0.03);	sampleOutput[23].push_back(0.86);	sampleOutput[23].push_back(0.09);

	sampleInput[24].push_back(0.5);		sampleInput[24].push_back(0.33);	sampleInput[24].push_back(0.75);
	sampleOutput[24].push_back(0.01);   sampleOutput[24].push_back(0.93);	sampleOutput[24].push_back(0.07);

	sampleInput[25].push_back(0.525);   sampleInput[25].push_back(0.5);		sampleInput[25].push_back(0.75);
	sampleOutput[25].push_back(0.01);   sampleOutput[25].push_back(0.93);	sampleOutput[25].push_back(0.06);

	sampleInput[26].push_back(0.55);	sampleInput[26].push_back(0.83);	sampleInput[26].push_back(0.75);
	sampleOutput[26].push_back(0.01);	sampleOutput[26].push_back(0.93);	sampleOutput[26].push_back(0.06);

	/* Internet Nymph */
	sampleInput[27].push_back(0.9);		sampleInput[27].push_back(0.13);    sampleInput[27].push_back(0.2);
	sampleOutput[27].push_back(0.09);   sampleOutput[27].push_back(0.09);	sampleOutput[27].push_back(0.8);

	sampleInput[28].push_back(0.925);	sampleInput[28].push_back(0.18);	sampleInput[28].push_back(0.25);
	sampleOutput[28].push_back(0.1);	sampleOutput[28].push_back(0.09);	sampleOutput[28].push_back(0.8);

	sampleInput[29].push_back(0.975);	sampleInput[29].push_back(0.17);	sampleInput[29].push_back(0.5);
	sampleOutput[29].push_back(0.05);	sampleOutput[29].push_back(0.10);	sampleOutput[29].push_back(0.85);

	sampleInput[30].push_back(0.975);	sampleInput[30].push_back(0.2);		sampleInput[30].push_back(0.8);
	sampleOutput[30].push_back(0.01);	sampleOutput[30].push_back(0.08);	sampleOutput[30].push_back(0.9);

	sampleInput[31].push_back(1);		sampleInput[31].push_back(0.22);	sampleInput[31].push_back(0.85);
	sampleOutput[31].push_back(0.01);   sampleOutput[31].push_back(0.07);	sampleOutput[31].push_back(0.9);

	sampleInput[32].push_back(1);		sampleInput[32].push_back(0.25);	sampleInput[32].push_back(1);
	sampleOutput[32].push_back(0.07);   sampleOutput[32].push_back(0.04);	sampleOutput[32].push_back(0.9);

	sampleInput[33].push_back(0.9);		sampleInput[33].push_back(0.22);	sampleInput[33].push_back(1);
	sampleOutput[33].push_back(0.01);	sampleOutput[33].push_back(0.07);	sampleOutput[33].push_back(0.9);

	/* Custom value */
	sampleInput[34].push_back(0);		sampleInput[34].push_back(0);		sampleInput[34].push_back(0);
	sampleOutput[34].push_back(1);		sampleOutput[34].push_back(0);		sampleOutput[34].push_back(0);

	sampleInput[35].push_back(1);		sampleInput[35].push_back(1);		sampleInput[35].push_back(1);
	sampleOutput[35].push_back(0);		sampleOutput[35].push_back(0);		sampleOutput[35].push_back(1);

	sampleInput[36].push_back(0.8);		sampleInput[36].push_back(0.6);		sampleInput[36].push_back(0.55);
	sampleOutput[36].push_back(0.05);	sampleOutput[36].push_back(0.15);	sampleOutput[36].push_back(0.8);

	sampleInput[37].push_back(0.9);		sampleInput[37].push_back(0.65);	sampleInput[37].push_back(0.6);
	sampleOutput[37].push_back(0);		sampleOutput[37].push_back(0.1);	sampleOutput[37].push_back(0.9);

    samples learnValueList;
    for (int i = 0; i < BP_TRAIN_NUMBER; i++)
    {
        sample sampleInAndOut;
        sampleInAndOut.inputValue = sampleInput[i];
        sampleInAndOut.outputValue = sampleOutput[i];
        learnValueList.push_back(sampleInAndOut);
    }

    return learnValueList;
}

void ServiceCalcThread::run()
{
    clock_t printStartTime = clock();

    ServiceUartThread *uartThread = new ServiceUartThread;
    uartThread->start();

    while (1)
    {
        clock_t startGATime = clock();
        GAPopulation *population = new GAPopulation(m_oLearnValueList);
        for (int i = 0; i < GA_GENERATIONS_NUMBER; i++)
        {
            /* Genetic algorithm */
            population->selection();
            population->cross();
            population->mutate();

            clock_t printEndTime = clock();
            if (PRINT_TIME_TEN < (printEndTime - printStartTime) / clock_t(2))
            {
                printStartTime = printEndTime;
                qDebug() << "the " << i << "th generation finish !";
            }
        }
        population->setBestIndividual();                
		clock_t endGATime = clock();
        qDebug() << "GA algorithm spend " << (endGATime - startGATime) / CLOCKS_PER_SEC << " s";  //Print out the GA algorithm spend time

        clock_t startBPTime = clock();
        BPNeuralNetworks *neuralNetwork = population->getBestIndividual();
        neuralNetwork->training(m_oLearnValueList, BP_RATE_THRESHOLD , BP_ERROR_THRESHOLD, true);
        clock_t endBPTime = clock();
        qDebug() << "BP Neural Network spend "  << (endBPTime - startBPTime) / CLOCKS_PER_SEC << " s";  //Print out the BP Neural Network spend time
        
        /* Get server status, restart if not successful */
        if (!neuralNetwork->getServiceStatus())
        {
            qDebug() << "Service params error! Reboot the service now!";
			population = nullptr;
        }
        else
		{
            /* Service begin, set the collector & BP network */
			uartThread->setCollector(m_pCollector);
            uartThread->setBPNeuralNetworks(neuralNetwork);
            break;
        }
    }
}