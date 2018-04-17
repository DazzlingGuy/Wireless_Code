#pragma once
/*
* @file     GABPNeuralNetworks.h
* @brief    GABPNeuralNetworks
* @author   Ren SiLin
* @date     2018-03-22
* @modify   2018-03-25
* @remarks
* @Copyright (c) 2018-2019 rensl Corporation
*/

#ifndef _GABPNEURALNETWORKS_H_
#define _GABPNEURALNETWORKS_H_

#include <QObject>
#include "GABPConsts.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <time.h>

using namespace std;

struct sample;

typedef vector<double> doubles;
typedef vector<sample> samples;

//Generate a -1 - 1 random number
inline double randomGeneration()
{
    return (((2.0 * (double)rand() / RAND_MAX) - 1) / 2.0);
}

//Activation function
inline double sigmoid(double x)
{
    double ans = 1 / (1 + exp(-x));
    return ans;
}

//Binary coding of weights and deviations
typedef struct BPNeuralNetworksParams
{
    int hiddenWeightBit[BP_INPUT_NODE][BP_HIDDEN_NODE];
    int outWeightBit[BP_HIDDEN_NODE][BP_OUTPUT_NODE];
    int hiddenBiasBit[BP_HIDDEN_NODE];
    int outBiasBit[BP_OUTPUT_NODE];
}BPNeuralNetworksParams;

typedef struct BPInputNode
{
    double value;
    doubles weight;
    doubles changeWeight;
}BPInputNode;

typedef struct BPOutputNode
{
    double value, delta, expectValue, bias, changeBias;
}BPOutputNode;

typedef struct BPHiddenNode
{
    double value, delta, bias, changeBias;
    doubles weight;
    doubles changeWeight;
}BPHiddenNode;

typedef struct sample
{
    doubles inputValue;
    doubles outputValue;
}sample;

class BPNeuralNetworks : public QObject
{
    Q_OBJECT
public:
    BPNeuralNetworks(QObject *parent);
    ~BPNeuralNetworks();

public:
    void initInputLayer();
    void initHiddenLayer();
    void initOutputLayer();
	void initLayerChangeData();

    void sendForwardPropagation();  //Single sample forward propagation
    void sendBackPropagation();     //Single sample back propagation

    void training(const samples sampleGroup, const double rateThresholdValue, const double errorThresholdValue, const bool bothMatch);  //Update weight & bias
    void predict(samples &testGroup);   //Neural network prediction

    void setInput(const doubles sampleIn);  //Set up learning sample input
    void setOutput(const doubles sampleOut);      //Set up learning sample output

	bool getServiceStatus();

    BPInputNode * getBPInputNode(const int index);
    BPOutputNode * getBPOutputNode(const int index);
    BPHiddenNode * getBPHiddenNode(const int layer, const int index);

    BPNeuralNetworksParams *getParams();

private:
    int m_learnCount;       //Current learning times

    double m_dCurrentFinalRate;
    double m_dCurrentFinalError;
    double m_dPreFinalRate;
    double m_dPreFinalError;

	bool m_bIsServiceRun;

    BPNeuralNetworksParams *m_pParams;

    BPInputNode *m_pInputLayer[BP_INPUT_NODE];							// Input layer
    BPOutputNode *m_pOutputLayer[BP_OUTPUT_NODE];                       // Output layer
    BPHiddenNode *m_pHiddenLayer[BP_HIDDEN_LAYER][BP_HIDDEN_NODE];		// Hidden layer
};

class GAIndividual : public QObject
{
    Q_OBJECT
public:
	GAIndividual(QObject *parent, const GAIndividual *origenIndividual, const samples inputAndOutputList);
	GAIndividual(QObject *parent, const samples inputAndOutputList);
    ~GAIndividual();

    void encode();
    void decode(); 

    void recalcFitness(const samples inputAndOutputList);
    void calcFitness();
    double getFitness() const;

    int getBitString(const int type, const int from, const int to = 0) const;
    void setBitString(const int value, const int type, const int from, const int to = 0);

    BPNeuralNetworks *getNetworks() const;
	void setBPNeuralNetworks(BPNeuralNetworks *newNetworks);

private:
    BPNeuralNetworks *m_pGABPNeuralNetwork;
	double m_individualFitness;
	samples m_sInputAndOutput;
};

class GAPopulation : public QObject
{
    Q_OBJECT
public:
    GAPopulation(samples inputAndOutputList);
    ~GAPopulation();

    void selection();
    void cross();
    void mutate();

    void setBestIndividual();
    double getBestFitness();
	BPNeuralNetworks * getBestIndividual();
    GAIndividual * getGAPopulation(const int index);

private:
    GAIndividual *m_oPopulationGroup[GA_POPULATION_SCALE];
    BPNeuralNetworks *m_pBestGABPNeuralNetwork;
	double m_bestFitness;
	samples m_sInputAndOutputList;
};

#endif  //_GABPNEURALNETWORKS_H_