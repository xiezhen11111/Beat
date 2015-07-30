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

	//��1��totalWeight�����ѡ��һ����㣬������
	int choice = random_range(1, totalWeight);
	int minInclusive = 1;
	int maxExclusive = minInclusive;
	float decisionWeight;

	WeightedDecision *weightedDecision;
	CCObject *obj;
	//ѭ������������Щ������weightֵ�ֶΣ�������choice�����䵽����һ��������
	//��ȻҲ����ʹ��һ�ּ򵥵İ취����ʹ��if-else���жϣ����������ַ�������
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
		if(_targetSprite->_actionState != kActionStateDead) //���Ŀ���Ѿ��������л�״̬
		{
			//����һ�³���
			float xTarget = _targetSprite->getPositionX();
			float xControl = _controlledSprite->getPositionX();
			if (xControl >= xTarget) //��������Ӣ���ұ�
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
	else if (_decision == kDecisionChase) //����������ҷ���׷�٣�׷���˺ܿ��ܾ��ǹ���
	{
		//lbl->setString("decision: chase");
		//��������˹������ܵ���ľ��룬Ŀ���ɫ������+�����ߵĹ�����ƫ��+�����ķ�Χ
		float reachDistance = _targetSprite->getCenterToSides() + _controlledSprite->_attackPoints[0].offset.x +
			_controlledSprite->_attackPoints[0].radius;
		//������Ҫȥ���ĵط��������е�Ȼ��Ŀ���targetSprite��λ�ã�������ֻ�赽ȭͷ�����ܹ�����Ӣ�۵ĵط������ˣ������ϻ���ֻ������ܵ�����Ӣ�������غϵط�������
		//���Ǿͳ�����+/- reachDistance����Ŀ�����߻��ұߣ�
		CCPoint reachPosition = ccp(_targetSprite->_groundPosition.x + (random_sign * reachDistance), 
			_targetSprite->_groundPosition.y);
		//��ɫҪ�ƶ��ķ���
		CCPoint moveDirection = ccpNormalize(ccpSub(reachPosition, _controlledSprite->_groundPosition));
		CCLOG("LLL---%d---%d",moveDirection.x,moveDirection.y);
		//�ƶ���ɫ
		_controlledSprite->walkWithDirection(moveDirection);

		//����һ���ߵ�Ҫȥ�ĵص���Ҫ��ʱ�䣬��Ҫ�߹���
		float len = ccpDistance(reachPosition, _controlledSprite->getPosition());
		float time = len / _controlledSprite->getWalkSpeed();
		if(time > 1.f) //�������������ҹ�Զ�������ż���õ�ʱ���ߣ��������Ѿ��ƶ��˺�Զ��λ�û����˻�ɵɵ����ԭ��Ŀ���н�
			_decisionDuration = frandom_range(0.5f,1.f);
		else
			_decisionDuration = time;
		
	}
	else if (_decision == kDecisionMove) //��������Ŀ���ƶ�,�����ڻ���������ҽ�Զ�������
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
		//1 �жϾ���
		float distanceSQ = ccpDistanceSQ(_controlledSprite->_groundPosition, _targetSprite->_groundPosition);
		float planeDist = fabsf(_controlledSprite->getShadow()->getPositionY() -
			_targetSprite->getShadow()->getPositionY()); //y�����ϵ�ƽ������ֵ
		float combinedRadius = _controlledSprite->_detectionRadius + _targetSprite->_detectionRadius;

		bool samePlane = false;
		bool canReach = false;
		bool tooFar = true;
		bool canMove = false;

		//2 ������״̬ʱ��ζ�ű����ƵĶ�������ƶ���
		if (_controlledSprite->_actionState == kActionStateWalk ||
			_controlledSprite->_actionState == kActionStateIdle)
		{
			canMove = true;
		}

		if (canMove)
		{
			//measure distances ������̽�ⷶΧ��
			if (distanceSQ <= combinedRadius * combinedRadius)
			{
				tooFar = false;

				//3 �Ƿ�ͬһƽ��
				if (fabsf(planeDist) <= kPlaneHeight)
				{
					samePlane = true;
					//check if any attack points can reach the target's contact points
					//�����Ǽ����ƶ���Ĺ���Բ�Ƿ�������Ŀ�����ĽӴ�Բ������һ��������ײ����Ԥ�ⷽ����
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

			// 4 ������ƶ���������Ŀ���ƶ���canReachΪtrue���ɵ������ô��ͣ��
			if (canReach && _decision == kDecisionChase)
			{
				this->_decision = kDecisionStayPut;
			}

			// 5 û������ʱ��
			if (_decisionDuration > 0)
			{
				_decisionDuration -= dt;
			}
			else //���˾���ʱ��
			{
				// 6
				if (tooFar) //���̫Զ, ���ڿ��к�׷����ѡ����Ҫ��׷��Ŀ��
				{
					this->_decision = this->decideWithAttackWeight(0, 20, 80, 0);
				}
				else
				{
					//7
					if (samePlane) //��ͬһƽ����
					{
						if (canReach)  //�ܹ���������Ҫ�ǹ���
						{ 
							this->_decision = this->decideWithAttackWeight(90, 5, 0, 5);
						}
						else //����������׷��
						{
							this->_decision = this->decideWithAttackWeight(0, 20, 50, 30);
						}
					}
					else //����ͬһƽ����
					{
						this->_decision = this->decideWithAttackWeight(0, 50, 40, 10);
					}
				}
				//���ݾ��������ж�
				this->setDecision(_decision);
			}
		}
	}
}