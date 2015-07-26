#ifndef _ArtificialIntelligence_
#define _ArtificialIntelligence_

#include "cocos2d.h"
#include "ActionSprite.h"
#include "WeightedDecision.h"

class ArtificialIntelligence : public cocos2d::CCObject
{
public:
	ArtificialIntelligence();
	~ArtificialIntelligence();

	static ArtificialIntelligence* aiWithControlledSprite(ActionSprite *controlledSprite, ActionSprite *targetSprite);
	bool initWithControlledSprite(ActionSprite *controlledSprite, ActionSprite *targetSprite);

	//����Ȩ����������
	AIDecision decideWithAttackWeight(int attackWeight, int idleWeight, int chaseWeight, int moveWeight);

	//��AI���ݾ������ƽ�ɫ������Ϊ
	void setDecision(AIDecision decision);

	void update(float dt);
protected:
	WeightedDecision *_attackDecision;
	WeightedDecision *_idleDecision;
	WeightedDecision *_chaseDecision;
	WeightedDecision *_moveDecision;

	float _decisionDuration;         //һ������ά�ֵ�ʱ��
	ActionSprite *_controlledSprite; //��AI���ݵĽ�ɫ
	ActionSprite *_targetSprite;     //AI�빥���Ķ���
	AIDecision _decision;		    //��ǰAI�����ľ���
	//�����ĸ����õľ��������飬�⽫ʹ���Ժ�����׵ı�������
	cocos2d::CCArray *_availableDecisions;  //��ǰ���õľ�������
private:
};
#endif