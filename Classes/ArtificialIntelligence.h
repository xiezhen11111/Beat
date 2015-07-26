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

	//根据权重作出决定
	AIDecision decideWithAttackWeight(int attackWeight, int idleWeight, int chaseWeight, int moveWeight);

	//让AI根据决定控制角色作出行为
	void setDecision(AIDecision decision);

	void update(float dt);
protected:
	WeightedDecision *_attackDecision;
	WeightedDecision *_idleDecision;
	WeightedDecision *_chaseDecision;
	WeightedDecision *_moveDecision;

	float _decisionDuration;         //一个决定维持的时间
	ActionSprite *_controlledSprite; //被AI操纵的角色
	ActionSprite *_targetSprite;     //AI想攻击的对象
	AIDecision _decision;		    //当前AI作出的决定
	//保存四个可用的决定的数组，这将使它以后很容易的遍历决策
	cocos2d::CCArray *_availableDecisions;  //当前可用的决定数组
private:
};
#endif