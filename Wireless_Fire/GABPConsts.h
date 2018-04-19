#pragma once
/*
* @file     GABPConsts.h
* @brief    Define the BP Consts
* @author   Ren SiLin
* @date     2018-03-22
* @remarks  
* @Copyright (c) 2018-2019 rensl Corporation
*/

#ifndef _GABPCONSTS_H_
#define _GABPCONSTS_H_

/* BP consts */
#define BP_INPUT_NODE       3       //Input layer node number
#define BP_HIDDEN_NODE      7       //Hidden layer node number
#define BP_HIDDEN_LAYER     1       //Hidden layer number
#define BP_OUTPUT_NODE      3       //Output layer node number
#define BP_LEARNING_RATE    0.9     //Learn rate
#define BP_ZERO             0.f  
#define BP_MAX              100.f

#define BP_TRAIN_NUMBER		20
#define BP_TESE_NUMBER		6

#define BP_RATE_THRESHOLD	0.0001
#define BP_ERROR_THRESHOLD	0.01

/* GA consts */
#define GA_POPULATION_SCALE         100
#define GA_GENERATIONS_NUMBER       100000
#define GA_CROSS_MUTATION_CARDINAL  100
#define GA_PC                       85
#define GA_PM                       15

#define GA_MAX_WEIGHT               1 
#define GA_MIN_WEIGHT               -1 

#define GA_BIT_COUNT                16
#define GA_RANGE                    (1 << GA_BIT_COUNT)
#define GA_ENCODE_STYLE(x)          ((x) * (GA_RANGE - 1)) / (GA_MAX_WEIGHT - GA_MIN_WEIGHT)
#define GA_DECODE_STYLE(x)          ((double)(x) * (double)(GA_MAX_WEIGHT - GA_MIN_WEIGHT) / (double)(GA_RANGE - 1))

enum GAParamsType {
    GAHiddenWeightBit = 0,
    GAOutWeightBit = 1,
    GAHiddenBias = 2,
    GAOutBias = 3
};

/* Print time consts */
#define PRINT_TIME_THREE            3000
#define PRINT_TIME_FIVE             5000
#define PRINT_TIME_TEN              10000

#endif  //_GABPCONSTS_H_