#pragma once
/*
* @file     FuzzyReasoning.h
* @brief    FuzzyReasoning
* @author   Ren SiLin
* @date     2018-04-7
* @remarks
* @Copyright (c) 2018-2019 rensl Corporation
*/

#ifndef _FUZZYREASONING_H_
#define _FUZZYREASONING_H_

#include <QObject>
#include "FuzzyConsts.h"
#include <iostream>
#include <string>
#include <cmath>
#include <vector>

using namespace std;

typedef struct FuzzyRule
{
    double isFireRate;
    double likeFireRate;
    double time;
    double finalStatus;
}FuzzyRule;

typedef vector<FuzzyRule> FuzzyRules;

inline double gaussian(double x, double fuzzyType)
{
    double ans = exp(-pow((x - fuzzyType), 2) / 0.2);
    return ans;
}

class FuzzyReasoning : public QObject
{
    Q_OBJECT
public:
    FuzzyReasoning();
    ~FuzzyReasoning();

    double triangleMembership(const double value, const double min = 0.0, const double middle = 0.5, const double max = 1.0);   //Do nothing.
    double normalMembership(const double value);

    void setMembershipRule(const double fuzzyRuleList[][FUZZY_RULE_COLUMN]);

    double finalDecision(FuzzyRule &fuzzySingleRule);


private:
    FuzzyRules m_oFuzzyRules;
};

#endif //_FUZZYREASONING_H_