#ifndef _WeightedDecision_
#define _WeightedDecision_

#include "cocos2d.h"
#include "Defines.h"

//根据权重(百分比机率)来作决定的AI，它只存储决策类型和权重
class WeightedDecision : public cocos2d::CCObject
{
public:
	WeightedDecision();
	~WeightedDecision();

	static WeightedDecision* decisionWithDecision(AIDecision decision, float weight); //作决定
	bool initWithDecision(AIDecision decision, float weight); //初始化决定

	AIDecision _decision; //要作出的决定，表示机器人可以选择的可能的动作
	float _weight;        //权重

protected:
	
private:
};
#endif