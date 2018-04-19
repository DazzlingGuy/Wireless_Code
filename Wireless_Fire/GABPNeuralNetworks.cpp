/*
* @file     GABPNeuralNetworks.cpp
* @brief    GABPNeuralNetworks
* @author   Ren SiLin
* @date     2018-03-22
* @remarks
* @Copyright (c) 2018-2019 rensl Corporation
*/

#include "GABPNeuralNetworks.h"
#include <QDebug>

BPNeuralNetworks::BPNeuralNetworks(QObject *parent) : QObject(parent),
    m_dCurrentFinalRate(BP_MAX),
    m_dCurrentFinalError(BP_MAX),
    m_dPreFinalRate(BP_MAX),
    m_dPreFinalError(BP_MAX),
	m_bIsServiceRun(false),
    m_learnCount(0)
{
    m_pParams = new BPNeuralNetworksParams(); 
    
    //Initialization value and memory allocation
    initInputLayer();
    initHiddenLayer();
    initOutputLayer();
}

BPNeuralNetworks::~BPNeuralNetworks()
{
    delete m_pParams;
	m_pParams = nullptr;

    for (int i = 0; i < BP_INPUT_NODE; i++)
    {
        delete m_pInputLayer[i];
		m_pInputLayer[i] = nullptr;
    }

    for (int i = 0; i < BP_HIDDEN_LAYER; i++)
    {
        if (i == BP_HIDDEN_LAYER - 1)
        {
            for (int j = 0; j < BP_HIDDEN_NODE; j++)
            {
                delete m_pHiddenLayer[i][j];
				m_pHiddenLayer[i][j] = nullptr;
            }
        }
        else
        {
            for (int j = 0; j < BP_HIDDEN_NODE; j++)
            {
                delete m_pHiddenLayer[i][j];
				m_pHiddenLayer[i][j] = nullptr;
            }
        }
    }

    for (int i = 0; i < BP_OUTPUT_NODE; i++)
    {
        delete m_pOutputLayer[i];
		m_pOutputLayer[i] = nullptr;
    }
}

//Initialize the input layer
void BPNeuralNetworks::initInputLayer()
{
    for (int i = 0; i < BP_INPUT_NODE; i++)
    {
        m_pInputLayer[i] = new BPInputNode();
        for (int j = 0; j < BP_HIDDEN_NODE; j++)
        {
            m_pInputLayer[i]->weight.push_back(randomGeneration());
            m_pInputLayer[i]->changeWeight.push_back(BP_ZERO);
        }
    }
}

//Initialize the hidden layer
void BPNeuralNetworks::initHiddenLayer()
{
    for (int i = 0; i < BP_HIDDEN_LAYER; i++)
    {
        if (i == BP_HIDDEN_LAYER - 1)
        {
            for (int j = 0; j < BP_HIDDEN_NODE; j++)
            {
                m_pHiddenLayer[i][j] = new BPHiddenNode();
                m_pHiddenLayer[i][j]->bias = randomGeneration();
                for (int k = 0; k < BP_OUTPUT_NODE; k++)
                {
                    m_pHiddenLayer[i][j]->weight.push_back(randomGeneration());
                    m_pHiddenLayer[i][j]->changeWeight.push_back(BP_ZERO);
                }
            }
        }
        else
        {
            for (int j = 0; j < BP_HIDDEN_NODE; j++)
            {
                m_pHiddenLayer[i][j] = new BPHiddenNode();
                m_pHiddenLayer[i][j]->bias = randomGeneration();
                for (int k = 0; k < BP_HIDDEN_NODE; k++)
                {
                    m_pHiddenLayer[i][j]->weight.push_back(randomGeneration());
                    m_pHiddenLayer[i][j]->changeWeight.push_back(BP_ZERO);
                }
            }
        }
    }
}

//Initialize the output layer
void BPNeuralNetworks::initOutputLayer()
{
    for (int i = 0; i < BP_OUTPUT_NODE; i++)
    {
        m_pOutputLayer[i] = new BPOutputNode();
        m_pOutputLayer[i]->bias = randomGeneration();
    }
}

//Clear deviation cache
void BPNeuralNetworks::initLayerChangeData()
{
    // Clear deviation cache delat & weight
    for (int i = 0; i < BP_INPUT_NODE; i++)
    {
        m_pInputLayer[i]->changeWeight.assign(m_pInputLayer[i]->changeWeight.size(), BP_ZERO);
    }

    for (int i = 0; i < BP_HIDDEN_LAYER; i++)
    {
        for (int j = 0; j < BP_HIDDEN_NODE; j++)
        {
            m_pHiddenLayer[i][j]->changeWeight.assign(m_pHiddenLayer[i][j]->changeWeight.size(), BP_ZERO);
            m_pHiddenLayer[i][j]->changeBias = BP_ZERO;
        }
    }
    for (int i = 0; i < BP_OUTPUT_NODE; i++)
    {
        m_pOutputLayer[i]->changeBias = BP_ZERO;
    }
}

void BPNeuralNetworks::sendForwardPropagation()
{
    //Forward to the hidden layer
    for (int i = 0; i < BP_HIDDEN_LAYER; i++)
    {
        if (i == 0)
        {
            for (int j = 0; j < BP_HIDDEN_NODE; j++)
            {
                double sum = BP_ZERO;
                for (int k = 0; k < BP_INPUT_NODE; k++)
                {
                    sum += m_pInputLayer[k]->value * m_pInputLayer[k]->weight[j];
                }
                sum += m_pHiddenLayer[i][j]->bias;
                m_pHiddenLayer[i][j]->value = sigmoid(sum);
            }
        }
        else
        {
            for (int j = 0; j < BP_HIDDEN_NODE; j++)
            {
                double sum = BP_ZERO;
                for (int k = 0; k < BP_HIDDEN_NODE; k++)
                {
                    sum += m_pHiddenLayer[i - 1][k]->value * m_pHiddenLayer[i - 1][k]->weight[j];
                }
                sum += m_pHiddenLayer[i][j]->bias;
                m_pHiddenLayer[i][j]->value = sigmoid(sum);
            }
        }
    }

    //Forward to the output layer
    for (int i = 0; i < BP_OUTPUT_NODE; i++)
    {
        double sum = BP_ZERO;
        for (int j = 0; j < BP_HIDDEN_NODE; j++)
        {
            sum += m_pHiddenLayer[BP_HIDDEN_LAYER - 1][j]->value * m_pHiddenLayer[BP_HIDDEN_LAYER - 1][j]->weight[i];
        }
        sum += m_pOutputLayer[i]->bias;
        m_pOutputLayer[i]->value = sigmoid(sum);
    }
}

void BPNeuralNetworks::sendBackPropagation()
{
    //Calculate the output layer deviation delta
    for (int i = 0; i < BP_OUTPUT_NODE; i++)
    {
        double error = m_pOutputLayer[i]->value - m_pOutputLayer[i]->expectValue;

        m_dCurrentFinalRate += error * error / 2;
        m_dCurrentFinalError += fabs(error);

        m_pOutputLayer[i]->delta = (m_pOutputLayer[i]->value - m_pOutputLayer[i]->expectValue) * (1 - m_pOutputLayer[i]->value) * m_pOutputLayer[i]->value;
    }

    //Pass back to the hidden layer to calculate hidden layer related data
    for (int i = BP_HIDDEN_LAYER - 1; i >= 0; i--)
    {
        if (i == BP_HIDDEN_LAYER - 1)
        {
            for (int j = 0; j < BP_HIDDEN_NODE; j++)
            {
                double sum = BP_ZERO;
                for (int k = 0; k < BP_OUTPUT_NODE; k++)
                {
                    sum += m_pOutputLayer[k]->delta * m_pHiddenLayer[i][j]->weight[k];
                }
                m_pHiddenLayer[i][j]->delta = sum * (1 - m_pHiddenLayer[i][j]->value) * m_pHiddenLayer[i][j]->value;
            }
        }
        else
        {
            for (int j = 0; j < BP_HIDDEN_NODE; j++)
            {
                double sum = BP_ZERO;
                for (int k = 0; k < BP_HIDDEN_NODE; k++)
                {
                    sum += m_pHiddenLayer[i + 1][k]->delta * m_pHiddenLayer[i][j]->weight[k];
                }
                m_pHiddenLayer[i][j]->delta = sum * (1 - m_pHiddenLayer[i][j]->value) * m_pHiddenLayer[i][j]->value;
            }
        }
    }

    //Return hidden layer offsets to the input layer to update weights
    for (int i = 0; i < BP_INPUT_NODE; i++)
    {
        for (int j = 0; j < BP_HIDDEN_NODE; j++)
        {
            //Since this is the hidden layer returned by the last layer, all directly take 0;
            m_pInputLayer[i]->changeWeight[j] += m_pInputLayer[i]->value * m_pHiddenLayer[0][j]->delta;
        }
    }

    //Update hidden layer weight & bias
    for (int i = 0; i < BP_HIDDEN_LAYER; i++)
    {
        if (i == BP_HIDDEN_LAYER - 1)
        {
            for (int j = 0; j < BP_HIDDEN_NODE; j++)
            {
                m_pHiddenLayer[i][j]->changeBias += m_pHiddenLayer[i][j]->delta;
                for (int k = 0; k < BP_OUTPUT_NODE; k++)
                {
                    m_pHiddenLayer[i][j]->changeWeight[k] += m_pHiddenLayer[i][j]->value * m_pOutputLayer[k]->delta;
                }
            }
        }
        else
        {
            for (int j = 0; j < BP_HIDDEN_NODE; j++)
            {
                m_pHiddenLayer[i][j]->changeBias += m_pHiddenLayer[i][j]->delta;
                for (int k = 0; k < BP_HIDDEN_NODE; k++)
                {
                    m_pHiddenLayer[i][j]->changeWeight[k] += m_pHiddenLayer[i][j]->value * m_pHiddenLayer[i + 1][k]->delta;
                }
            }
        }
    }

    //Update output layer deviation changeBias
    for (int i = 0; i < BP_OUTPUT_NODE; i++)
    {
        m_pOutputLayer[i]->changeBias += m_pOutputLayer[i]->delta;
    }
}

void BPNeuralNetworks::training(samples learnValueList, double rateThresholdValue, double errorThresholdValue, bool bothMatch)
{
	m_bIsServiceRun = true;
    size_t learnValueCount = learnValueList.size();

    clock_t printStartTime = clock();
    clock_t changeRateStartTime = clock();
    double learnRate = BP_LEARNING_RATE;

    //Judgment of stopping conditions can be judged using thresholds
    //Or it can be judged using training times.
    while (1)
    {
        if (bothMatch)
        {
            if (m_dCurrentFinalRate < rateThresholdValue && m_dCurrentFinalError < errorThresholdValue)
            {
                break;
            }
        }
        else
        {
            if (m_dCurrentFinalRate < rateThresholdValue || m_dCurrentFinalError < errorThresholdValue)
            {
                break;
            }
        }

        m_learnCount++;

        clock_t printEndTime = clock();
        if (PRINT_TIME_TEN < (printEndTime - printStartTime) / clock_t(3))
        {
            printStartTime = printEndTime;
            qDebug() << "The " << m_learnCount << " time training finished, finalErrorRate = " << m_dCurrentFinalRate << ", finalError = " << m_dCurrentFinalError;
        }

        m_dCurrentFinalRate = BP_ZERO;
        m_dCurrentFinalError = BP_ZERO;

        initLayerChangeData();

        //Start training
        for (int iter = 0; iter < learnValueCount; iter++)
        {
            setInput(learnValueList[iter].inputValue);
            setOutput(learnValueList[iter].outputValue);

            sendForwardPropagation();
            sendBackPropagation();
        }

        //Update the input layerweight
        for (int i = 0; i < BP_INPUT_NODE; i++)
        {
            for (int j = 0; j < BP_HIDDEN_NODE; j++)
            {
                m_pInputLayer[i]->weight[j] -= learnRate * m_pInputLayer[i]->changeWeight[j] / learnValueCount;
            }
        }

        //Update hidden layer weight & bias
        for (int i = 0; i < BP_HIDDEN_LAYER; i++)
        {
            if (i == BP_HIDDEN_LAYER - 1)
            {
                for (int j = 0; j < BP_HIDDEN_NODE; j++)
                {
                    // bias
                    m_pHiddenLayer[i][j]->bias -= learnRate * m_pHiddenLayer[i][j]->changeBias / learnValueCount;

                    // weight
                    for (int k = 0; k < BP_OUTPUT_NODE; k++)
                    {
                        m_pHiddenLayer[i][j]->weight[k] -= learnRate * m_pHiddenLayer[i][j]->changeWeight[k] / learnValueCount;
                    }
                }
            }
            else
            {
                for (int j = 0; j < BP_HIDDEN_NODE; j++)
                {
                    // bias
                    m_pHiddenLayer[i][j]->bias -= learnRate * m_pHiddenLayer[i][j]->changeBias / learnValueCount;

                    // weight
                    for (int k = 0; k < BP_HIDDEN_NODE; k++)
                    {
                        m_pHiddenLayer[i][j]->weight[k] -= learnRate * m_pHiddenLayer[i][j]->changeWeight[k] / learnValueCount;
                    }
                }
            }
        }

        //Update the output layer bias
        for (int i = 0; i < BP_OUTPUT_NODE; i++)
        {
            m_pOutputLayer[i]->bias -= learnRate * m_pOutputLayer[i]->changeBias / learnValueCount;
        }

        m_dCurrentFinalError = m_dCurrentFinalError / (learnValueCount * BP_OUTPUT_NODE);
		m_dCurrentFinalRate = m_dCurrentFinalRate / (learnValueCount * BP_OUTPUT_NODE);

		clock_t changeRateEndTime = clock();
		if (PRINT_TIME_TEN < (changeRateEndTime - changeRateStartTime) / clock_t(3))
		{
			changeRateStartTime = changeRateEndTime;

			double finalRatePercent  = (double)(m_dPreFinalRate - m_dCurrentFinalRate) / m_dCurrentFinalRate;
			double finalErrorPercent = (double)(m_dPreFinalError - m_dCurrentFinalError) / m_dCurrentFinalError;

			if (0.01 >= finalRatePercent && 0.01 >= finalErrorPercent)
            {
                if ((2 * rateThresholdValue >= m_dCurrentFinalRate) && (2 * errorThresholdValue >= m_dCurrentFinalError))
                {
                    m_bIsServiceRun = true;
                }
                else
                {
                    m_bIsServiceRun = false;
                }
                break;
			}

			m_dPreFinalError = m_dCurrentFinalError;
			m_dPreFinalRate  = m_dCurrentFinalRate;
		}
	}

    qDebug() << "End training";
}

void BPNeuralNetworks::predict(samples& testValueList)
{
    size_t testValueCount = testValueList.size();

    initLayerChangeData();

    for (int it = 0; it < testValueCount; it++)
    {
        setInput(testValueList[it].inputValue);

        //Forward data to the hidden layer
        //If there are multiple hidden layers that continue to transmit
        for (int i = 0; i < BP_HIDDEN_LAYER; i++)
        {
            if (i == 0)
            {
                for (int j = 0; j < BP_HIDDEN_NODE; j++)
                {
                    double sum = BP_ZERO;
                    for (int k = 0; k < BP_INPUT_NODE; k++)
                    {
                        sum += m_pInputLayer[k]->value * m_pInputLayer[k]->weight[j];
                    }
                    sum += m_pHiddenLayer[i][j]->bias;
                    m_pHiddenLayer[i][j]->value = sigmoid(sum);
                }
            }
            else
            {
                for (int j = 0; j < BP_HIDDEN_NODE; j++)
                {
                    double sum = BP_ZERO;
                    for (int k = 0; k < BP_HIDDEN_NODE; k++)
                    {
                        sum += m_pHiddenLayer[i - 1][k]->value * m_pHiddenLayer[i - 1][k]->weight[j];
                    }
                    sum += m_pHiddenLayer[i][j]->bias;
                    m_pHiddenLayer[i][j]->value = sigmoid(sum);
                }
            }
        }

        //Forward data to the output layer
        for (int i = 0; i < BP_OUTPUT_NODE; i++)
        {
            double sum = BP_ZERO;
            for (int j = 0; j < BP_HIDDEN_NODE; j++)
            {
                sum += m_pHiddenLayer[BP_HIDDEN_LAYER - 1][j]->value * m_pHiddenLayer[BP_HIDDEN_LAYER - 1][j]->weight[i];
            }
            sum += m_pOutputLayer[i]->bias;
            m_pOutputLayer[i]->value = sigmoid(sum);
            testValueList[it].outputValue.push_back(m_pOutputLayer[i]->value);
        }
    }
}

void BPNeuralNetworks::setInput(doubles sampleInput)
{
    for (int i = 0; i < BP_INPUT_NODE; i++)
    {
        m_pInputLayer[i]->value = sampleInput[i];
    }
}

void BPNeuralNetworks::setOutput(const doubles sampleOutput)
{
    for (int i = 0; i < BP_OUTPUT_NODE; i++)
    {
        m_pOutputLayer[i]->expectValue = sampleOutput[i];
    }
}

bool BPNeuralNetworks::getServiceStatus()
{
	return m_bIsServiceRun;
}

BPInputNode * BPNeuralNetworks::getBPInputNode(int index)
{
    return m_pInputLayer[index];
}

BPOutputNode * BPNeuralNetworks::getBPOutputNode(int index)
{
    return m_pOutputLayer[index];
}


BPHiddenNode * BPNeuralNetworks::getBPHiddenNode(int layer, int index)
{
    return m_pHiddenLayer[layer][index];
}

BPNeuralNetworksParams * BPNeuralNetworks::getParams()
{
    return m_pParams;
}

GAIndividual::GAIndividual(QObject *parent, const GAIndividual *origenIndividual, const samples inputAndOutputList) : QObject(parent),
    m_sInputAndOutput(inputAndOutputList)
{
	m_pGABPNeuralNetwork = new BPNeuralNetworks(this);

    //Copy the data of the original individual into the binary code of the new individual
    for (int i = 0; i < BP_INPUT_NODE; i++)
    {
        for (int j = 0; j < BP_HIDDEN_NODE; j++)
        {
            m_pGABPNeuralNetwork->getParams()->hiddenWeightBit[i][j] = origenIndividual->getNetworks()->getParams()->hiddenWeightBit[i][j];
        }
    }
    for (int i = 0; i < BP_HIDDEN_NODE; i++)
    {
        for (int j = 0; j < BP_OUTPUT_NODE; j++)
        {
            m_pGABPNeuralNetwork->getParams()->outWeightBit[i][j] = origenIndividual->getNetworks()->getParams()->outWeightBit[i][j];
        }
    }
    for (int i = 0; i < BP_HIDDEN_NODE; i++)
    {
        m_pGABPNeuralNetwork->getParams()->hiddenBiasBit[i] = origenIndividual->getNetworks()->getParams()->hiddenBiasBit[i];
    }
    for (int i = 0; i < BP_OUTPUT_NODE; i++)
    {
        m_pGABPNeuralNetwork->getParams()->outBiasBit[i] = origenIndividual->getNetworks()->getParams()->outBiasBit[i];
    }
    decode();
    recalcFitness(inputAndOutputList);
}

GAIndividual::GAIndividual(QObject *parent, const samples inputAndOutputList) : QObject(parent),
	m_sInputAndOutput(inputAndOutputList)
{
	m_pGABPNeuralNetwork = new BPNeuralNetworks(this);

    recalcFitness(inputAndOutputList);
	encode();
}

GAIndividual::~GAIndividual()
{
    delete m_pGABPNeuralNetwork;
	m_pGABPNeuralNetwork = nullptr;
}

void GAIndividual::calcFitness()
{
    for (int i = 0; i < BP_OUTPUT_NODE; i++)
    {
        double error = m_pGABPNeuralNetwork->getBPOutputNode(i)->value - m_pGABPNeuralNetwork->getBPOutputNode(i)->expectValue;
        m_individualFitness += error * error / 2;
    }
}

void GAIndividual::recalcFitness(const samples inputAndOutputList)
{
    m_individualFitness = 0;

    //Recalculate fitness
    for (int i = 0; i < inputAndOutputList.size(); i++)
    {
        m_pGABPNeuralNetwork->setInput(inputAndOutputList[i].inputValue);
        m_pGABPNeuralNetwork->setOutput(inputAndOutputList[i].outputValue);
        m_pGABPNeuralNetwork->sendForwardPropagation();
        calcFitness();
    }
    m_individualFitness = 1 / m_individualFitness;
}

void GAIndividual::encode()
{
    //Binary coding of layers
	for (int i = 0; i < BP_INPUT_NODE; i++)
	{
		for (int j = 0; j < BP_HIDDEN_NODE; j++)
		{
			int tempEncode = (int)GA_ENCODE_STYLE(m_pGABPNeuralNetwork->getBPInputNode(i)->weight[j]);
			m_pGABPNeuralNetwork->getParams()->hiddenWeightBit[i][j] = tempEncode;
		}
	}
    for (int i = 0; i < BP_HIDDEN_NODE; i++)
    {
        int tempEncode = (int)GA_ENCODE_STYLE(m_pGABPNeuralNetwork->getBPHiddenNode(0, i)->bias);
        m_pGABPNeuralNetwork->getParams()->hiddenBiasBit[i] = tempEncode;
    }
	for (int i = 0; i < BP_HIDDEN_NODE; i++)
	{
		for (int j = 0; j < BP_OUTPUT_NODE; j++)
		{
			int tempEncode = (int)GA_ENCODE_STYLE(m_pGABPNeuralNetwork->getBPHiddenNode(0, i)->weight[j]);
			m_pGABPNeuralNetwork->getParams()->outWeightBit[i][j] = tempEncode;
		}
	}
    for (int i = 0; i < BP_OUTPUT_NODE; i++)
    {
        int tempEncode = (int)GA_ENCODE_STYLE(m_pGABPNeuralNetwork->getBPOutputNode(i)->bias);
        m_pGABPNeuralNetwork->getParams()->outBiasBit[i] = tempEncode;
    }
}

void GAIndividual::decode()
{
    //Binary decoding of layers
	for (int i = 0; i < BP_INPUT_NODE; i++)
	{
		for (int j = 0; j < BP_HIDDEN_NODE; j++)
		{
			double tempValue = GA_DECODE_STYLE(m_pGABPNeuralNetwork->getParams()->hiddenWeightBit[i][j]);
			m_pGABPNeuralNetwork->getBPInputNode(i)->weight[j] = tempValue;
		}
	}
    for (int i = 0; i < BP_HIDDEN_NODE; i++)
    {
        double tempValue = GA_DECODE_STYLE(m_pGABPNeuralNetwork->getParams()->hiddenBiasBit[i]);
        m_pGABPNeuralNetwork->getBPHiddenNode(0, i)->bias = tempValue;
    }
	for (int i = 0; i < BP_HIDDEN_NODE; i++)
	{
		for (int j = 0; j < BP_OUTPUT_NODE; j++)
		{
			double tempValue = GA_DECODE_STYLE(m_pGABPNeuralNetwork->getParams()->outWeightBit[i][j]);
			m_pGABPNeuralNetwork->getBPHiddenNode(0, i)->weight[j] = tempValue;
		}
	}
    for (int i = 0; i < BP_OUTPUT_NODE; i++)
    {
        double tempValue = GA_DECODE_STYLE(m_pGABPNeuralNetwork->getParams()->outBiasBit[i]);
        m_pGABPNeuralNetwork->getBPOutputNode(i)->bias = tempValue;
    }
}

double GAIndividual::getFitness() const
{
    return m_individualFitness;
}

int GAIndividual::getBitString(const int type, const int from, const int to) const
{
    //Get different types of binary data
    if (GAHiddenWeightBit == type)
    {
		return m_pGABPNeuralNetwork->getParams()->hiddenWeightBit[from][to];
    }
    else if (GAOutWeightBit == type)
    {
        return m_pGABPNeuralNetwork->getParams()->outWeightBit[from][to];
    }
    else if (GAHiddenBias == type)
    {
        return m_pGABPNeuralNetwork->getParams()->hiddenBiasBit[from];
    }
    else if (GAOutBias == type)
    {
        return m_pGABPNeuralNetwork->getParams()->outBiasBit[from];
    }

    return -1;
}

void GAIndividual::setBitString(const int value, const int type, const int from, const int to)
{
    //Set different types of binary data
    //Recalculate fitness
    if (GAHiddenWeightBit == type)
    {
        m_pGABPNeuralNetwork->getParams()->hiddenWeightBit[from][to] = value;
    }
    else if (GAOutWeightBit == type)
    {
        m_pGABPNeuralNetwork->getParams()->outWeightBit[from][to] = value;
    }
    else if (GAHiddenBias == type)
    {
        m_pGABPNeuralNetwork->getParams()->hiddenBiasBit[from] = value;
    }
    else if (GAOutBias == type)
    {
        m_pGABPNeuralNetwork->getParams()->outBiasBit[from] = value;
    }
    decode();
    recalcFitness(m_sInputAndOutput);
}

BPNeuralNetworks * GAIndividual::getNetworks() const
{
    return m_pGABPNeuralNetwork;
}

void GAIndividual::setBPNeuralNetworks(BPNeuralNetworks *newNetworks)
{
	m_pGABPNeuralNetwork = newNetworks;
}

GAPopulation::GAPopulation(samples inputAndOutputList) :
	m_sInputAndOutputList(inputAndOutputList)
{
    //New individual
    srand(unsigned(time(0)));
    for (int i = 0; i < GA_POPULATION_SCALE; i++)
    {
        m_oPopulationGroup[i] = new GAIndividual(this, inputAndOutputList);
    }
}

GAPopulation::~GAPopulation()
{
    for (int i = 0; i < GA_POPULATION_SCALE; i++)
    {
		delete m_oPopulationGroup[i];
		m_oPopulationGroup[i] = nullptr;
    }
}

void GAPopulation::selection()
{
    double selectionProbality[GA_POPULATION_SCALE];
    double onLineProbality[GA_POPULATION_SCALE];
    double totalFitness = 0;

    //Initialization probability to straight line
    for (int i = 0; i < GA_POPULATION_SCALE; i++)
    {
        totalFitness += m_oPopulationGroup[i]->getFitness();
    }
    for (int i = 0; i < GA_POPULATION_SCALE; i++)
    {
        selectionProbality[i] = float(m_oPopulationGroup[i]->getFitness()) / float(totalFitness);
    }
    onLineProbality[0] = selectionProbality[0];
    for (int i = 1; i < GA_POPULATION_SCALE; i++)
    {
        onLineProbality[i] = onLineProbality[i - 1] + selectionProbality[i];
    }
    if (onLineProbality[GA_POPULATION_SCALE - 1] < 1)
    {
        onLineProbality[GA_POPULATION_SCALE - 1] = 1;
    }

    GAIndividual *tempIndividualGroup[GA_POPULATION_SCALE];

    //Roulette
    for (int i = 0; i < GA_POPULATION_SCALE; i++)
    {
        float temp = ((float)rand()) / (float)RAND_MAX;
		int j = 0;
        for (j; j < GA_POPULATION_SCALE; j++)
        {
            if (temp <= onLineProbality[j])
            {
                break;
            }
			if (j >= GA_POPULATION_SCALE)
			{
                qDebug() << "rand error!";
			}
        }
		tempIndividualGroup[i] = m_oPopulationGroup[j];
    }
	for (int i = 0; i < GA_POPULATION_SCALE; i++)
	{
		bool needFree = true;
		for (int j = 0; j < GA_POPULATION_SCALE; j++)
		{
			if (tempIndividualGroup[j] == m_oPopulationGroup[i])
			{
				break;
			}
			if (GA_POPULATION_SCALE - 1 == j)
			{
				GAIndividual *needFreeIndividual = m_oPopulationGroup[i];
				delete needFreeIndividual;
				for (int index = i; index < GA_POPULATION_SCALE; index++)
				{
					if (m_oPopulationGroup[index] == needFreeIndividual)
					{
						m_oPopulationGroup[index] = NULL;
					}
				}
				needFreeIndividual = nullptr;
			}
		}
	}

    /* Copy info */
    for (int i = 0; i < GA_POPULATION_SCALE; i++)
    {
        m_oPopulationGroup[i] = tempIndividualGroup[i];
    }
}

void GAPopulation::cross()
{
    int crossCount = 0;
    if (int(GA_PC * GA_POPULATION_SCALE) % 2 == 0)
    {
        crossCount = int(float(GA_PC * GA_POPULATION_SCALE) / float(2 * GA_CROSS_MUTATION_CARDINAL));
    }
    else
    {
        crossCount = int(float(GA_PC * GA_POPULATION_SCALE) / float(2 * GA_CROSS_MUTATION_CARDINAL)) + 1;
    }

	double crossRate = (float)rand() / RAND_MAX;
    if (crossRate > ((float)GA_PC / (float)GA_CROSS_MUTATION_CARDINAL))
    {
        return;
    }

    //Swap a bit, swap if different
	srand(unsigned(time(0)));
    for (int i = 0; i < crossCount; i++)
    {
        GAIndividual *newIndividual1 = nullptr;
        GAIndividual *newIndividual2 = nullptr;
        int tempDot1 = rand() % GA_POPULATION_SCALE;
        int tempDot2 = rand() % GA_POPULATION_SCALE;
        for (int j = 0; j < BP_INPUT_NODE; j++)
        {
            for (int k = 0; k < BP_HIDDEN_NODE; k++)
            {
                int crossDot = rand() % GA_BIT_COUNT;

                if ((m_oPopulationGroup[tempDot1]->getBitString(GAHiddenWeightBit, j, k) & (1 << crossDot)) ^ (m_oPopulationGroup[tempDot2]->getBitString(GAHiddenWeightBit, j, k) & (1 << crossDot)))
                {
                    int weightBitString1 = m_oPopulationGroup[tempDot1]->getBitString(GAHiddenWeightBit, j, k) ^ (1 << crossDot);
                    int weightBitString2 = m_oPopulationGroup[tempDot2]->getBitString(GAHiddenWeightBit, j, k) ^ (1 << crossDot);

                    if (!newIndividual1 && !newIndividual2)
                    {
                        newIndividual1 = new GAIndividual(this, m_oPopulationGroup[tempDot1], m_sInputAndOutputList);
                        newIndividual2 = new GAIndividual(this, m_oPopulationGroup[tempDot2], m_sInputAndOutputList);
                    }
                    else
                    {
                        newIndividual1->setBitString(weightBitString1, GAHiddenWeightBit, j, k);
                        newIndividual2->setBitString(weightBitString2, GAHiddenWeightBit, j, k);
                    }
                }
                else
                {
                    continue;
                }
            }
        }

        for (int j = 0; j < BP_HIDDEN_NODE; j++)
        {
            for (int k = 0; k < BP_HIDDEN_NODE; k++)
            {
                int crossDot = rand() % GA_BIT_COUNT;

                if ((m_oPopulationGroup[tempDot1]->getBitString(GAOutWeightBit, j, k) & (1 << crossDot)) ^ (m_oPopulationGroup[tempDot2]->getBitString(GAOutWeightBit, j, k) & (1 << crossDot)))
                {
                    int weightBitString1 = m_oPopulationGroup[tempDot1]->getBitString(GAOutWeightBit, j, k) ^ (1 << crossDot);
                    int weightBitString2 = m_oPopulationGroup[tempDot2]->getBitString(GAOutWeightBit, j, k) ^ (1 << crossDot);

                    if (!newIndividual1 && !newIndividual2)
                    {
                        newIndividual1 = new GAIndividual(this, m_oPopulationGroup[tempDot1], m_sInputAndOutputList);
                        newIndividual2 = new GAIndividual(this, m_oPopulationGroup[tempDot2], m_sInputAndOutputList);
                    }
                    else
                    {
                        newIndividual1->setBitString(weightBitString1, GAOutWeightBit, j, k);
                        newIndividual2->setBitString(weightBitString2, GAOutWeightBit, j, k);
                    }
                }
                else
                {
                    continue;
                }
            }
        }
        if (newIndividual1 && newIndividual2)
        {
            if (newIndividual1->getFitness() > m_oPopulationGroup[tempDot1]->getFitness())
            {
                m_oPopulationGroup[tempDot1] = newIndividual1;
            }
            else
            {
				delete newIndividual1;
				newIndividual1 = nullptr;
            }

            if (newIndividual2->getFitness() > m_oPopulationGroup[tempDot2]->getFitness())
            {
                m_oPopulationGroup[tempDot2] = newIndividual2;
            }
            else
            {
				delete newIndividual2;
				newIndividual2 = nullptr;
            }
        }
     }
}

void GAPopulation::mutate(void)
{
    int mutateCount = int(float(GA_PM) / (float)GA_CROSS_MUTATION_CARDINAL * GA_POPULATION_SCALE * 2);

    double mutateRate = (float)rand() / RAND_MAX;
    if (mutateRate > ((float)GA_PM / (float)GA_CROSS_MUTATION_CARDINAL))
    {
        return;
    }

    //Mutating a bit
	srand(unsigned(time(0)));
    for (int i = 0; i < mutateCount; i++)
    {
        int mutateIndividualIndex = rand() % GA_POPULATION_SCALE;

        GAIndividual *newIndividual = nullptr;
        for (int j = 0; j < BP_INPUT_NODE; j++)
        {
            for (int k = 0; k < BP_HIDDEN_NODE; k++)
            {
                int mutateDot = rand() % GA_BIT_COUNT;
                int changeString = m_oPopulationGroup[mutateIndividualIndex]->getBitString(GAHiddenWeightBit, j, k) ^ (1 << mutateDot);
                if (!newIndividual)
                {
                    newIndividual = new GAIndividual(this, m_oPopulationGroup[mutateIndividualIndex], m_sInputAndOutputList);
                }
                else
                {
                    newIndividual->setBitString(changeString, GAHiddenWeightBit, j, k);
                }
            }
        }
		for (int j = 0; j < BP_HIDDEN_NODE; j++)
		{
            GAIndividual *newIndividual = nullptr;
			for (int k = 0; k < BP_OUTPUT_NODE; k++)
			{
				int mutateDot = rand() % GA_BIT_COUNT;
				int changeString = m_oPopulationGroup[mutateIndividualIndex]->getBitString(GAOutWeightBit, j, k) ^ (1 << mutateDot);
                if (!newIndividual)
                {
                    newIndividual = new GAIndividual(this, m_oPopulationGroup[mutateIndividualIndex], m_sInputAndOutputList);
                }
                else
                {
                    newIndividual->setBitString(changeString, GAOutWeightBit, j, k);
                }
			}
		}
        if (newIndividual)
        {
            if (newIndividual->getFitness() > m_oPopulationGroup[mutateIndividualIndex]->getFitness())
            {
                m_oPopulationGroup[mutateIndividualIndex] = newIndividual;
            }
            else
            {
                delete newIndividual;
				newIndividual = nullptr;
            }
        }
    }
}

void GAPopulation::setBestIndividual()
{
    int maxflag = 0;
	m_bestFitness = 0;
    for (int i = 1; i < GA_POPULATION_SCALE; i++)
    {
        if (m_oPopulationGroup[i]->getFitness() > m_oPopulationGroup[maxflag]->getFitness())
        {

            maxflag = i;
        }
    }
    for (int i = 1; i < GA_POPULATION_SCALE; i++)
    {
        if (m_oPopulationGroup[i] != m_oPopulationGroup[maxflag] && nullptr != m_oPopulationGroup[i])
        {
            delete m_oPopulationGroup[i];
			for (int j = i + 1; j < GA_POPULATION_SCALE; j++)
			{
				if (m_oPopulationGroup[i] == m_oPopulationGroup[j])
				{
					m_oPopulationGroup[j] = nullptr;
				}
			}
			m_oPopulationGroup[i] = nullptr;
        }
    }
    m_pBestGABPNeuralNetwork = m_oPopulationGroup[maxflag]->getNetworks();
	m_bestFitness = m_oPopulationGroup[maxflag]->getFitness();
}

BPNeuralNetworks * GAPopulation::getBestIndividual()
{
	return m_pBestGABPNeuralNetwork;
}

double GAPopulation::getBestFitness()
{
	return m_bestFitness;
}

GAIndividual * GAPopulation::getGAPopulation(const int index)
{
	return m_oPopulationGroup[index];
}