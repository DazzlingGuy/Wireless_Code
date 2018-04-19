/*
* @file     FuzzyReasoning.cpp
* @brief    FuzzyReasoning
* @author   Ren SiLin
* @date     2018-04-7
* @remarks
* @Copyright (c) 2018-2019 rensl Corporation
*/

#include "FuzzyReasoning.h"

FuzzyReasoning::FuzzyReasoning()
{
    const double fuzzyRuleList[FUZZY_RULE_ROW][FUZZY_RULE_COLUMN] = { { FUZZY_PS, FUZZY_PS, FUZZY_PS, FUZZY_PS },
																	  { FUZZY_PS, FUZZY_PM, FUZZY_PS, FUZZY_PS },
																	  { FUZZY_PS, FUZZY_PB, FUZZY_PS, FUZZY_PS },
																	  { FUZZY_PS, FUZZY_PS, FUZZY_PB, FUZZY_PS },
																	  { FUZZY_PS, FUZZY_PM, FUZZY_PB, FUZZY_PB },
																	  { FUZZY_PS, FUZZY_PB, FUZZY_PB, FUZZY_PB },
																	  { FUZZY_PS, FUZZY_PS, FUZZY_PS, FUZZY_PS },
																	  { FUZZY_PM, FUZZY_PM, FUZZY_PS, FUZZY_PB },
																	  { FUZZY_PM, FUZZY_PS, FUZZY_PB, FUZZY_PB },
																	  { FUZZY_PM, FUZZY_PM, FUZZY_PB, FUZZY_PB },
																	  { FUZZY_PB, FUZZY_PS, FUZZY_PS, FUZZY_PS },
																	  { FUZZY_PB, FUZZY_PS, FUZZY_PB, FUZZY_PB } };
    setMembershipRule(fuzzyRuleList);   //Fuzzy inference algorithm rules written
}

FuzzyReasoning::~FuzzyReasoning()
{
    cout << "FuzzyReasoning destruct!" << endl;
}

double FuzzyReasoning::triangleMembership(const double value, const double min, const double middle, const double max)
{
    double u;

    if (value >= min && value <= middle)
    {
        u = (value - min) / (middle - min);
    }
    else if (value > middle && value <= max)
    {
        u = (max - value) / (max - middle);
    }
    else
    {
        u = 0.0;
    }

    if (u >= 0.0 && u < 1 / 3.0)
    {
        return FUZZY_PS;
    }
    else if (u >= 1 / 3.0 && u < 2 / 3.0)
    {
        return FUZZY_PM;
    }
    else
    {
        return FUZZY_PB;
    }
}

double FuzzyReasoning::normalMembership(const double value)
{
    double PSMembership = gaussian(value, FUZZY_PS);
    double PMMembership = gaussian(value, FUZZY_PM);
    double PBMembership = gaussian(value, FUZZY_PB);

    /* Get the max menbership, such as PS, PM, PB */
    if (PSMembership > PMMembership)
    {
        if (PSMembership > PBMembership)
        {
            return FUZZY_PS;
        }
        else
        {
            return FUZZY_PM;
        }
    }
    else if (PMMembership > PBMembership)
    {
        return FUZZY_PM;
    }
    else
    {
        return FUZZY_PB;
    }
}

void FuzzyReasoning::setMembershipRule(const double fuzzyRuleList[][FUZZY_RULE_COLUMN])
{
    for (int i = 0; i < FUZZY_RULE_ROW; i++)
    {
        FuzzyRule fuzzySingleRule;
        fuzzySingleRule.isFireRate        = fuzzyRuleList[i][0];
        fuzzySingleRule.likeFireRate      = fuzzyRuleList[i][1];
        fuzzySingleRule.time              = fuzzyRuleList[i][2];
        fuzzySingleRule.finalStatus       = fuzzyRuleList[i][3];
        m_oFuzzyRules.push_back(fuzzySingleRule);
    }
}

double FuzzyReasoning::finalDecision(FuzzyRule &fuzzySingleRule)
{
    fuzzySingleRule.finalStatus = -1;

    for (vector<FuzzyRule>::iterator iter = m_oFuzzyRules.begin(); iter != m_oFuzzyRules.end(); iter++)
    {
        if ((iter->isFireRate == fuzzySingleRule.isFireRate) && (iter->likeFireRate == fuzzySingleRule.likeFireRate) && (iter->time == fuzzySingleRule.time))
        {
            if (FUZZY_PS == iter->finalStatus)
            {
                fuzzySingleRule.finalStatus = FUZZY_PS;
                break;
            }
            else if (FUZZY_PB == iter->finalStatus)
            {
                fuzzySingleRule.finalStatus = FUZZY_PB;
                break;
            }
        }
    }

    return fuzzySingleRule.finalStatus;
}