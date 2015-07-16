#ifndef _WeightedDecision_
#define _WeightedDecision_

#include "cocos2d.h"
#include "Defines.h"

//����Ȩ��(�ٷֱȻ���)����������AI
class WeightedDecision : public cocos2d::CCObject
{
public:
	WeightedDecision();
	~WeightedDecision();

	static WeightedDecision* decisionWithDecision(AIDecision decision, float weight); //������
	bool initWithDecision(AIDecision decision, float weight); //��ʼ������

	AIDecision _decision; //Ҫ�����ľ���
	float _weight;        //Ȩ��

protected:
	
private:
};
#endif