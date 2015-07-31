#include "ArtificialIntelligence.h"
#include "GameScene.h"

USING_NS_CC;

ArtificialIntelligence::ArtificialIntelligence()
{
	this->_controlledSprite = NULL;
	this->_targetSprite = NULL;
	this->_availableDecisions = NULL;
	_attackDecision = NULL;
	_idleDecision = NULL;
	_chaseDecision = NULL;
	_moveDecision = NULL;
}

ArtificialIntelligence::~ArtificialIntelligence()
{

}

ArtificialIntelligence* ArtificialIntelligence::aiWithControlledSprite(ActionSprite *controlledSprite, ActionSprite *targetSprite)
{
	ArtificialIntelligence* pRet = new ArtificialIntelligence();
	if (pRet && pRet->initWithControlledSprite(controlledSprite, targetSprite))
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

bool ArtificialIntelligence::initWithControlledSprite(ActionSprite *controlledSprite, ActionSprite *targetSprite)
{
	bool bRet = false;

	this->_controlledSprite = controlledSprite;
	this->_targetSprite = targetSprite;
	this->_availableDecisions = CCArray::createWithCapacity(4);
	this->_availableDecisions->retain();

	_attackDecision = WeightedDecision::decisionWithDecision(kDecisionAttack, 0.f);
	_idleDecision = WeightedDecision::decisionWithDecision(kDecisionStayPut, 0.f);
	_chaseDecision = WeightedDecision::decisionWithDecision(kDecisionChase, 0.f);
	_moveDecision = WeightedDecision::decisionWithDecision(kDecisionMove, 0.f);

	_availableDecisions->addObject(_attackDecision);
	_availableDecisions->addObject(_idleDecision);
	_availableDecisions->addObject(_chaseDecision);
	_availableDecisions->addObject(_moveDecision);

	_decisionDuration = 0;
	bRet = true;
	return bRet;
}

AIDecision ArtificialIntelligence::decideWithAttackWeight(int attackWeight, int idleWeight, int chaseWeight, int moveWeight)
{
	int totalWeight = attackWeight + idleWeight + chaseWeight + moveWeight;
	_attackDecision->_weight = attackWeight;
	_idleDecision->_weight = idleWeight;
	_chaseDecision->_weight = chaseWeight;
	_moveDecision->_weight = moveWeight;

	//在1到totalWeight里随机选择一个落点，即决定
	int choice = random_range(1, totalWeight);
	int minInclusive = 1;
	int maxExclusive = minInclusive;
	float decisionWeight;

	WeightedDecision *weightedDecision;
	CCObject *obj;
	//循环迭代，把这些决定按weight值分段，检查最后choice到底落到了哪一个区段内
	//当然也可以使用一种简单的办法就是使用if-else来判断，但不如这种方法巧妙
	CCARRAY_FOREACH(_availableDecisions, obj)
	{
		weightedDecision = (WeightedDecision *)obj;
		decisionWeight = weightedDecision->_weight;

		if (decisionWeight > 0)
		{
			maxExclusive = minInclusive + decisionWeight;

			if (choice >= minInclusive && choice < maxExclusive)
			{
				this->_decision = weightedDecision->_decision;
				return weightedDecision->_decision;
			}
		}
		minInclusive = maxExclusive;
	}

	return kDecisionNo;
}

void ArtificialIntelligence::setDecision(AIDecision decision)
{
	_decision = decision;
	//CCLabelTTF* lbl = GameScene::inst()->_hudLayer->_lblTest;

	if (_decision == kDecisionAttack)
	{
		//lbl->setString("decision: attack");
		if(_targetSprite->_actionState != kActionStateDead) //如果目标已经死亡，切换状态
		{
			//设置一下朝向
			float xTarget = _targetSprite->getPositionX();
			float xControl = _controlledSprite->getPositionX();
			if (xControl >= xTarget) //机器人在英雄右边
			{
				_controlledSprite->_directionX = -1.0f;	
			}
			else
				_controlledSprite->_directionX = 1.0f;
			_controlledSprite->setScaleX(_controlledSprite->_directionX * kScaleFactor);

			_controlledSprite->attack();
			_decisionDuration = frandom_range(0.25f, 1.0f);
		}
	}
	else if (_decision == kDecisionStayPut)
	{
		//lbl->setString("decision: idle");
		_controlledSprite->idle();
		_decisionDuration = frandom_range(0.25f, 1.f);
	}
	else if (_decision == kDecisionChase) //机器人向玩家发起追踪，追上了很可能就是攻击
	{
		//lbl->setString("decision: chase");
		//计算机器人攻击所能到达的距离，目标角色自身宽度+攻击者的攻击的偏移+攻击的范围
		float reachDistance = _targetSprite->getCenterToSides() + _controlledSprite->_attackPoints[0].offset.x +
			_controlledSprite->_attackPoints[0].radius;
		//机器人要去到的地方，理想中当然是目标的targetSprite的位置，但机器只需到拳头攻击能够得着英雄的地方就行了，否则老会出现机器人跑到主角英雄身体重合地方的现象
		//与是就出现了+/- reachDistance（到目标点左边或右边）
		CCPoint reachPosition = ccp(_targetSprite->_groundPosition.x + (random_sign * reachDistance), 
			_targetSprite->_groundPosition.y);
		//角色要移动的方向
		CCPoint moveDirection = ccpNormalize(ccpSub(reachPosition, _controlledSprite->_groundPosition));
		CCLOG("LLL---%d---%d",moveDirection.x,moveDirection.y);
		//移动角色
		_controlledSprite->walkWithDirection(moveDirection);

		//计算一下走到要去的地点需要的时间，不要走过了
		float len = ccpDistance(reachPosition, _controlledSprite->getPosition());
		float time = len / _controlledSprite->getWalkSpeed();
		if(time > 1.f) //如果机器人离玩家过远不能照着计算好的时间走，免得玩家已经移动了很远的位置机器人还傻傻的向原定目标行进
			_decisionDuration = frandom_range(0.5f,1.f);
		else
			_decisionDuration = time;
		
	}
	else if (_decision == kDecisionMove) //机器人向目标移动,多用于机器人离玩家较远的情况下
	{
		//lbl->setString("decision: move");
		float randomX = random_sign * frandom_range(20.f* kPointFactor, 100.0f * kPointFactor);
		float randomY = random_sign * frandom_range(10.f * kPointFactor, 40.f * kPointFactor);
		CCPoint randomPoint = ccp(_targetSprite->_groundPosition.x + randomX, _targetSprite->_groundPosition.y + randomY);
		CCPoint moveDirection = ccpNormalize(ccpSub(randomPoint, _controlledSprite->_groundPosition));
		_controlledSprite->walkWithDirection(moveDirection);

		_decisionDuration = frandom_range(0.25f, 0.5f);
	}
}

void ArtificialIntelligence::update(float dt)
{
	if (_targetSprite && _controlledSprite && _controlledSprite->_actionState>kActionStateNone)
	{
		//1 判断距离
		float distanceSQ = ccpDistanceSQ(_controlledSprite->_groundPosition, _targetSprite->_groundPosition);
		float planeDist = fabsf(_controlledSprite->getShadow()->getPositionY() -
			_targetSprite->getShadow()->getPositionY()); //y方向上的平面距离差值
		float combinedRadius = _controlledSprite->_detectionRadius + _targetSprite->_detectionRadius;

		bool samePlane = false;
		bool canReach = false;
		bool tooFar = true;
		bool canMove = false;

		//2 这两种状态时意味着被控制的对象可以移动了
		if (_controlledSprite->_actionState == kActionStateWalk ||
			_controlledSprite->_actionState == kActionStateIdle)
		{
			canMove = true;
		}

		if (canMove)
		{
			//measure distances 距离在探测范围内
			if (distanceSQ <= combinedRadius * combinedRadius)
			{
				tooFar = false;

				//3 是否同一平面
				if (fabsf(planeDist) <= kPlaneHeight)
				{
					samePlane = true;
					//check if any attack points can reach the target's contact points
					//下面是检测控制对象的攻击圆是否碰到了目标对象的接触圆（这是一种类似碰撞检测的预测方法）
					int attackPointCount = _controlledSprite->_attackPointCount;
					int contactPointCount = _targetSprite->_contactPointCount;

					int i, j;
					ContactPoint attackPoint, contactPoint;
					for (i=0; i<attackPointCount; i++)
					{
						attackPoint = _controlledSprite->_attackPoints[i];
						for (j=0; j<contactPointCount; j++)
						{
							contactPoint = _targetSprite->_contactPoints[j];
							combinedRadius = attackPoint.radius + contactPoint.radius;

							float distanceSQ = ccpDistanceSQ(attackPoint.position, contactPoint.position);
							if (distanceSQ <= combinedRadius * combinedRadius)
							{
								canReach = true;
								break;
							}
						}//end for j

						if (canReach)
						{
							break;
						}
					} //end for i
				}
			}//measure distances

			// 4 如果控制对象正在向目标移动且canReach为true（可到达），那么将停下
			if (canReach && _decision == kDecisionChase)
			{
				this->_decision = kDecisionStayPut;
			}

			// 5 没到决定时间
			if (_decisionDuration > 0)
			{
				_decisionDuration -= dt;
			}
			else //到了决定时间
			{
				// 6
				if (tooFar) //离的太远, 会在空闲和追踪中选择，主要是追踪目标
				{
					this->_decision = this->decideWithAttackWeight(0, 20, 80, 0);
				}
				else
				{
					//7
					if (samePlane) //在同一平面上
					{
						if (canReach)  //能够到，就主要是攻击
						{ 
							this->_decision = this->decideWithAttackWeight(90, 5, 0, 5);
						}
						else //够不到，再追踪
						{
							this->_decision = this->decideWithAttackWeight(0, 20, 50, 30);
						}
					}
					else //不在同一平面上
					{
						this->_decision = this->decideWithAttackWeight(0, 50, 40, 10);
					}
				}
				//根据决定作出行动
				this->setDecision(_decision);
			}
		}
	}
}