#include "WeightedDecision.h"
USING_NS_CC;

WeightedDecision::WeightedDecision()
{

}

WeightedDecision::~WeightedDecision()
{

}

WeightedDecision* WeightedDecision::decisionWithDecision(AIDecision decision, float weight)
{
	WeightedDecision* pRet = new WeightedDecision();
	if (pRet && pRet->initWithDecision(decision, weight))
	{
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return pRet;
	}
}

bool WeightedDecision::initWithDecision(AIDecision decision, float weight)
{
	bool bRet = false;

	_decision = decision;
	_weight = weight;

	bRet = true;
	return bRet;
}