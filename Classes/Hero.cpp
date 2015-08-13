#include "Hero.h"
#include "GameScene.h"
#include "Defines.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;

Hero::Hero()
{
	_attackTwoAction = NULL;
	_attackThreeAction = NULL;
	_weapon = NULL;
	_attackGroup = NULL;
	_attackTwoGroup = NULL;
	_attackThreeGroup = NULL;
	_idleGroup = NULL;
	_walkGroup = NULL;
}

Hero::~Hero()
{

}

bool Hero::init()
{
	bool bRet = false;
	do 
	{
		CC_BREAK_IF(!ActionSprite::initWithSpriteFrameName("hero_idle_00.png"));
		//���ý�ɫ��Χ��
		this->setCenterToBottom(39.0f * kPointFactor); //���ĵ㵽�ײ��ľ���
		this->setCenterToSides(29.0f * kPointFactor);  //���ĵ㵽���ߵľ���

		//��ײ�趨
		this->_detectionRadius = 100.f * kPointFactor;
		this->_attackPointCount = 3; //��ʼ��������ײ��Ϊ3��
		//����һ��յ�CP���൱�ڳ�ʼ����ײ���б�
		int i = 0;
		for (i=0; i<_attackPointCount; i++)
		{
			ContactPoint cp;
			_attackPoints.push_back(cp);
		}
		this->_contactPointCount = 4; //��ʼ��������ײ��Ϊ4��
		for (i=0; i<_contactPointCount; i++)
		{
			ContactPoint cp;
			_contactPoints.push_back(cp);
		}
		

		this->_maxHitPoints = 200.0f;
		this->_hitPoints = this->_maxHitPoints;
		this->_attackDamage = 5.0f;
		this->_attackForce = 0.f;//4.0f * kPointFactor;

		//int i;
		//��������idle����
		/*CCArray *idleFrames = CCArray::createWithCapacity(6);
		for (i=0; i<6; i++)
		{
			CCString* strName = CCString::createWithFormat("hero_idle_%02d.png",i);
			CCSpriteFrame *frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(strName->getCString());
			idleFrames->addObject(frame);
		}
		CCAnimation *idleAnimation = CCAnimation::createWithSpriteFrames(idleFrames, 1.0f/12.0f);*/
		CCAnimation *idleAnimation = this->animationWithPrefix("hero_idle", 0, 6, 1.0f/12.0f);
		_idleGroup = AnimateGroup::actionWithAnimation(idleAnimation, 1);
		this->setIdleAction(CCRepeatForever::create(CCRepeatForever::create(_idleGroup)));

		//����walk����
		CCAnimation *walkAnimation = this->animationWithPrefix("hero_walk", 0, 8, 1.0f/12.0f);
		_walkGroup = AnimateGroup::actionWithAnimation(walkAnimation, 1);
		this->setWalkAction(CCRepeatForever::create(CCRepeatForever::create(_walkGroup)));
		this->_walkSpeed = 80 * kPointFactor;

		//����run����
		CCAnimation *runAnimation = this->animationWithPrefix("hero_run", 0, 8, 1.0f/12.0f);
		this->setRunAction(CCRepeatForever::create(CCAnimate::create(runAnimation)));
		this->_runSpeed = 160 * kPointFactor;

		//������������
		CCArray *frames = CCArray::createWithCapacity(6);
		CCSpriteFrame *frame;
		for (int i=0; i<3; i++)
		{
			frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(CCString::createWithFormat("hero_attack_00_%02d.png", i)->getCString());
			frames->addObject(frame);
		}
		for(int i=1; i>=0; i--)
		{
			frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(CCString::createWithFormat("hero_attack_00_%02d.png", i)->getCString());
			frames->addObject(frame);
		}
		
		CCAnimation *attackAnimation = CCAnimation::createWithSpriteFrames(frames, 1.0f/15.0f);
		_attackGroup = AnimateGroup::actionWithAnimation(attackAnimation, 1);
		this->setAttackAction(CCSequence::create(_attackGroup, CCCallFunc::create(this,callfunc_selector(Hero::idle)), NULL));

		//��������
		CCArray* jumpRiseFrames = CCArray::createWithCapacity(2);
		jumpRiseFrames->addObject(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("hero_jump_05.png"));//05 00
		jumpRiseFrames->addObject(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("hero_jump_00.png"));
		CCAnimation* animation = CCAnimation::createWithSpriteFrames(jumpRiseFrames, 1.0f/12.0f);
		this->setJumpRiseAction(CCAnimate::create(animation));

		//���䶯��
		CCAnimation* fallAnimation = this->animationWithPrefix("hero_jump", 1, 4, 1.0f/12.0f);
		CCAnimate* animate = CCAnimate::create(fallAnimation);
		this->setJumpFallAction(animate);

		//��½����
		CCSequence *seq = CCSequence::create(CCCallFunc::create(this, callfunc_selector(Hero::setLandingDisplayFrame)),CCDelayTime::create(1.0f/12.0f),
			CCCallFunc::create(this, callfunc_selector(Hero::idle)),NULL);
		this->setJumpLandAction(seq);

		//hurt ����
		CCAnimation* hurtAnimation = this->animationWithPrefix("hero_hurt", 0, 3, 1.0f/12.0f);
		CCSequence* seq2 = CCSequence::create(CCAnimate::create(hurtAnimation), CCCallFunc::create(this, callfunc_selector(Hero::idle)), NULL);
		this->setHurtAction(seq2);

		//��������
		CCAnimation* knockedOutAnimation = this->animationWithPrefix("hero_knockout", 0, 5, 1.0f/12.0f);
		this->setKnockedOutAction(CCAnimate::create(knockedOutAnimation));

		//������������˸���μ���
		this->setDieAction(CCBlink::create(2.0f, 10.0f));

		//��������
		CCAnimation* recoverAnimation = this->animationWithPrefix("hero_getup", 0, 6, 1.0f/12.0f);
		CCSequence* seq3 = CCSequence::create(CCAnimate::create(recoverAnimation), CCCallFunc::create(this, callfunc_selector(Hero::jumpLand)), NULL);
		this->setRecoverAction(seq3);

		//��Ծ��������
		CCAnimation *jumpAttackAnimation = this->animationWithPrefix("hero_jumpattack", 0, 5, 1.0f/10.0f);
		CCSequence* seqJumpAttack = CCSequence::create(CCAnimate::create(jumpAttackAnimation), CCCallFunc::create(this, callfunc_selector(Hero::jumpFall)), NULL);
		this->setJumpAttackAction(seqJumpAttack);

		//�ܲ���������
		CCAnimation *runAttackAnimation = this->animationWithPrefix("hero_runattack", 0, 6, 1.0f/10.0f);
		CCSequence *runAttack = CCSequence::create(CCAnimate::create(runAttackAnimation), CCCallFunc::create(this, callfunc_selector(Hero::idle)), NULL);
		this->setRunAttackAction(runAttack);

		//���¹�������
		CCAnimation *attackTwoAnimation = this->animationWithPrefix("hero_attack_01", 0, 3, 1.f/12.f);
		_attackTwoGroup = AnimateGroup::actionWithAnimation(attackTwoAnimation, 1);
		CCSequence* seqAtkTwo = CCSequence::create(_attackTwoGroup, CCCallFunc::create(this, callfunc_selector(Hero::idle)), NULL);
		this->setAttackTwoAction(seqAtkTwo);

		//���¹�������
		CCAnimation *attackThreeAnimation = this->animationWithPrefix("hero_attack_02", 0, 5, 1.f/10.f);
		_attackThreeGroup = AnimateGroup::actionWithAnimation(attackThreeAnimation, 1);
		CCSequence* seqAtkThree = CCSequence::create(_attackThreeGroup, CCCallFunc::create(this, callfunc_selector(Hero::idle)), NULL);
		this->setAttackThreeAction(seqAtkThree);

		//�������������
		
		//1.�����˶����������Ĺ����ȵ�ʱ��
		_attackTwoDelayTime = kOneAtkTime;
		_attackThreeDelayTime = 0.45f;
		
		//2.�����ļ��ʱ�䣬��ʱ�����ٴι����Ļ��ᴥ������
		_chainTimer = 0;

		//3.���������Ĺ�����
		this->_attackTwoDamage = 10.f;
		this->_attackThreeDamage = 20.f;


		this->_jumpAttackDamage = 15.0f;
		this->_runAttackDamage = 15.0f;

		CCSprite* shadow = CCSprite::createWithSpriteFrameName("shadow_character.png");
		shadow->setOpacity(190);
		this->setShadow(shadow);	
		
		this->_directionX = 1.0;

		_recoveryRate = 5.0;
		_hurtLimit = 20.0;
		_hurtToLerance = _hurtLimit;
		_attackDelayTime = kOneAtkTime;

		//this->setActionState(kActionStateIdle);

		bRet = true;
		

	} while (0);
	return bRet;
}

void Hero::setLandingDisplayFrame()
{
	this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("hero_jump_05.png"));
}

void Hero::setContactPointsForAction(ActionState actionState)
{
	float dir = this->_directionX; //ƫ��Ҫ���ǽ�ɫ����
	switch (actionState)
	{
	case kActionStateIdle:
		this->modifyContactPointAtIndex(0, ccp(3.0f, 23.0f), 19.0f);
		this->modifyContactPointAtIndex(1, ccp(17.0f, 10.0f), 10.0f);
		this->modifyContactPointAtIndex(2, CCPointZero, 19.0f);
		this->modifyContactPointAtIndex(3, ccp(0.0f, -21.0f), 20.0f);

		this->_attackPointCount = 0;   // idle״̬ʱû�й����ж�
		break;
	case kActionStateWalk:
		this->modifyContactPointAtIndex(0, ccp(8.0f, 23.0f), 19.0f);
		this->modifyContactPointAtIndex(1, ccp(12.0f, 4.0f), 4.0f);
		this->modifyContactPointAtIndex(2, CCPointZero, 10.0f);
		this->modifyContactPointAtIndex(3, ccp(0.0f, -21.0f), 20.0f);

		this->_attackPointCount = 0;   // idle״̬ʱû�й����ж�

		break;
	case kActionStateAttack:
		this->modifyContactPointAtIndex(0, ccp(15.0f, 23.0f), 19.0f);
		this->modifyContactPointAtIndex(1, ccp(24.5f, 4.0f), 6.0f);
		this->modifyContactPointAtIndex(2, CCPointZero, 16.0f);
		this->modifyContactPointAtIndex(3, ccp(0.0f, -21.0f), 20.0f);

		this->modifyAttackPointAtIndex(0, ccp(41.0f, 3.0f), 10.0f);
		this->modifyAttackPointAtIndex(1, ccp(41.0f, 3.0f), 10.0f);
		this->modifyAttackPointAtIndex(2, ccp(41.0f, 3.0f), 10.0f);

		this->_attackPointCount = 3;   // ��3�������ж�
		break;
	case kActionStateAttackTwo:
		this->modifyAttackPointAtIndex(0, ccp(51.6f, 2.4f), 13.0f);
		this->modifyAttackPointAtIndex(1, ccp(51.6f, 2.4f), 13.0f);
		this->modifyAttackPointAtIndex(2, ccp(51.6f, 2.4f), 13.0f);

		this->_attackPointCount = 3;   // ��3�������ж�
		break;
	case kActionStateAttackThree:
		this->modifyAttackPointAtIndex(0, ccp(61.8f, 6.2f), 22.0f);
		this->modifyAttackPointAtIndex(1, ccp(61.8f, 6.2f), 22.0f);
		this->modifyAttackPointAtIndex(2, ccp(61.8f, 6.2f), 22.0f);

		this->_attackPointCount = 3;   // ��3�������ж�
		break;
	case kActionStateRunAttack:
		this->modifyAttackPointAtIndex(0, ccp(31.2f, -8.8f), 10.0f);
		this->modifyAttackPointAtIndex(1, ccp(31.2f, -8.8f), 10.0f);
		this->modifyAttackPointAtIndex(2, ccp(31.2f, -8.8f), 10.0f);

		this->_attackPointCount = 3;   // ��3�������ж�
		break;
	case kActionStateJumpAttack:
		this->modifyAttackPointAtIndex(2, ccp(70.0f, -55.0f), 8.0f);
		this->modifyAttackPointAtIndex(1, ccp(55.0f, -42.0f), 12.0f);
		this->modifyAttackPointAtIndex(0, ccp(34.0f, -25.0f), 17.0f);

		this->_attackPointCount = 3;   // ��3�������ж�
		break;
	default:
		break;
	}
	
}

void Hero::setDisplayFrame(cocos2d::CCSpriteFrame *pNewFrame)
{
	CCSprite::setDisplayFrame(pNewFrame);

	//��⵱ǰ���ŵ��ǲ��ǹ���֡�������ǹ����������������Ĺ���֡
	CCSpriteFrame *attackFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("hero_attack_00_01.png");
	CCSpriteFrame *runAttackFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("hero_runattack_02.png");
	CCSpriteFrame *runAttackFrame2 = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("hero_runattack_03.png");
	CCSpriteFrame *jumpAttackFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("hero_jumpattack_02.png");

	//new frames �����Ĺ���֡
	CCSpriteFrame *attackFrame2 = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("hero_attack_01_01.png");
	CCSpriteFrame *attackFrame3 = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("hero_attack_02_02.png");

	if (pNewFrame == attackFrame || pNewFrame==attackFrame2)
	{
		if(this->_delegate->actionSpriteDidAttack(this)) //����true��ʾ��ɫ��������������
		{
			//�������ˣ����������Ĵ���ʱ��
			_chainTimer = 0.3f;   //����ǵ�һ����ڶ�����Ҫ��������һ����Чʱ��

			if (_weapon)
			{
				_weapon->used();
			}
		}
	}
	else if (pNewFrame == attackFrame3)
	{
		//this->_delegate->actionSpriteDidAttack(this);
		if (this->_delegate->actionSpriteDidAttack(this)) //�ɹ������˹�����Ϊ
		{
			if (_weapon)
			{
				_weapon->used();
			}
		}
	}
	else if (pNewFrame == runAttackFrame||pNewFrame == runAttackFrame2
		||pNewFrame == jumpAttackFrame)
	{
		this->_delegate->actionSpriteDidAttack(this); //����ģʽ��������Ϊ
	}
}

void Hero::hurtWithDamage(float damage, float force, cocos2d::CCPoint direction)
{
	if (_weapon)
	{
		this->dropWeapon();
	}
	ActionSprite::hurtWithDamage(damage, force, direction);
	
	if (this->_actionState == kActionStateHurt)
	{
		
		_hurtToLerance -= damage;
		if (_hurtToLerance <= 0)
		{
			//��������
			this->knockoutWithDamage(0, direction);
		}
	}
}

void Hero::knockoutWithDamage(float damage, cocos2d::CCPoint direction)
{
	if (_weapon)
	{
		this->dropWeapon();
	}
	ActionSprite::knockoutWithDamage(damage, direction);
	if (this->_actionState == kActionStateKnockedOut)
	{
		_hurtToLerance = _hurtLimit;
		if (this->_hitPoints<=0)
		{
			SimpleAudioEngine::sharedEngine()->playEffect("Sounds/herodeath.caf");
		}
	}
}

void Hero::update(float dt)
{
	ActionSprite::update(dt);

	//��������Чʱ����£��ڴ���0ʱ���ڻᴥ��������������Ϊ��ͨ������
	if (_chainTimer > 0)
	{
		_chainTimer -= dt;
		if (_chainTimer <= 0)
		{
			_chainTimer = 0;
		}
	}

	if (_hurtToLerance <_hurtLimit)
	{
		//����������ִ���޴ˣ������Ļ�ֻ��������εĻ���Ӣ�۲Żᱻ����
		_hurtToLerance += _hurtLimit * dt / _recoveryRate;
		if(_hurtToLerance >= _hurtLimit)
			_hurtToLerance = _hurtLimit;
	}

	/*CCLabelTTF* lblTest = GameScene::inst()->_hudLayer->_lblTest;
	CCString* str = CCString::createWithFormat("state: %d", _actionState);
	lblTest->setString(str->getCString());*/
}

void Hero::attack()
{

	/*
	�������ԭ��������ʱ����Ϊ0.3s�������˵�һ�α�������ʱ�򣬻�������һ�������ļ��Ϊ0.3s�������һ��������0.3s�ڷ����˾ͻᴥ������������������Ϊ��ͨ��������
	ͬ�������Ҳ������������γ�����������ø���Ĺ���������Ҫע�����_actionDelay�������ĵȴ�ʱ�䣩��ֵҪС��_chainTimer��������Ҳ��Ч��
	*/

	//���ݵ�ǰ�Ĺ���״̬���Ƿ�����������Чʱ�乥�������ж��Ƿ񴥷������������Ľ׶�

	//�жϵ�ǰ�Ĺ���״̬�����������еڼ�����_chainTimer����������Чʱ�䣬���˾Ͳ��ᴥ��
	if (this->_actionState == kActionStateAttack && _chainTimer>0)
	{
		//�����ڶ�������
		_chainTimer = 0;//����setDisplayFrame�м�⵽��������ֵ��0.3��
		this->stopAllActions();
		this->runAction(_attackTwoAction);
		this->setActionState(kActionStateAttackTwo);
		_actionDelay = _attackTwoDelayTime;//���¹����ĵȴ�ʱ�䣨����0��ʱ�򣬹�����Ч��
	}
	else if (this->_actionState == kActionStateAttackTwo && _chainTimer>0)
	{
		//��������������
		_chainTimer = 0;
		this->stopAllActions();
		this->runAction(_attackThreeAction);
		this->setActionState(kActionStateAttackThree);
		_actionDelay = _attackThreeDelayTime;
	}
	else//���������Ĺ���
		ActionSprite::attack();
}

void Hero::getUp()
{
	ActionSprite::getUp();
	//����һ��ʱ���޵�
	this->setInvincibleState(1.f);
}

void Hero::cleanup()
{
	this->_attackTwoAction = NULL;
	this->_attackThreeAction = NULL;

	ActionSprite::cleanup();
}

bool Hero::pickUpWeapon(Weapon *weapon)
{
	if (this->_actionState == kActionStateIdle)
	{
		this->stopAllActions();
		weapon->pickedUp();
		this->setLandingDisplayFrame();
		//scheduleOnce(schedule_selector(Hero::setWeapon), 1.f/12.f);
																		    //CCCallFuncND::create(this, callfuncND_selector(GameLayer::triggerEvent),(void*)kEventStateFreeWalk)
		CCSequence *seq = CCSequence::create(CCDelayTime::create(1.f/12.f), CCCallFuncND::create(this, callfuncND_selector(Hero::setWeapon),(void *)(weapon)), NULL);
		this->runAction(seq);

		return true;
	}

	return false;
}

void Hero::removeAllAnimationMembers()
{
	_attackGroup->_members->removeAllObjects();
	_attackTwoGroup->_members->removeAllObjects();
	_attackThreeGroup->_members->removeAllObjects();
	_idleGroup->_members->removeAllObjects();
	_walkGroup->_members->removeAllObjects();
}

void Hero::setWeapon(CCNode *pSender, void* data)
{
	Weapon* weapon = (Weapon *)data;
	this->stopAllActions();
	if(_weapon)
		this->removeAllAnimationMembers();

	_weapon = weapon;
	if (_weapon)
	{
		_weapon->setDelegate(this);
		_weapon->setScaleX(this->getScaleX());
		_attackGroup->_members->addObject(_weapon->_attack);
		_attackTwoGroup->_members->addObject(_weapon->_attackTwo);
		_attackThreeGroup->_members->addObject(_weapon->_attackThree);
		_idleGroup->_members->addObject(_weapon->_idle);
		_walkGroup->_members->addObject(_weapon->_walk);
	}

	this->runAction(this->getIdleAction());
	this->_velocity = CCPointZero;
	this->setActionState(kActionStateIdle);
	_actionDelay = 0.0f;
}

void Hero::dropWeapon()
{
	Weapon *weapon = _weapon;
	//�������Ӷ����б����Ƴ�
	if (_weapon)
	{
		_attackGroup->_members->removeObject(_weapon->_attack);
		_attackTwoGroup->_members->removeObject(_weapon->_attackTwo);
		_attackThreeGroup->_members->removeObject(_weapon->_attackThree);
		_idleGroup->_members->removeObject(_weapon->_idle);
		_walkGroup->_members->removeObject(_weapon->_walk);

		_weapon->setDelegate(NULL);
	}
	this->_weapon = NULL;
	weapon->droppedFrom(this->_groundPosition.y-this->_shadow->getPositionY(), this->_shadow->getPosition());
}

void Hero::weaponDidReachLimit(Weapon* weapon)
{
	this->dropWeapon();
	//this->_weapon = NULL;
	//weapon->droppedFrom(this->_groundPosition.y-this->_shadow->getPositionY(), this->_shadow->getPosition());
}

void Hero::setPosition(const cocos2d::CCPoint& pos)
{
	ActionSprite::setPosition(pos);
	if (_weapon)
	{
		_weapon->setPosition(pos);
	}
	//CCLog("Hero.x:%f, Hero.y:%f",pos.x, pos.y);
}

void Hero::setScaleX(float fScaleX)
{
	ActionSprite::setScaleX(fScaleX);
	if (_weapon)
	{
		_weapon->setScaleX(fScaleX);
	}
}

void Hero::setScaleY(float fScaleY)
{
	ActionSprite::setScaleY(fScaleY);
	if (_weapon)
	{
		_weapon->setScaleY(fScaleY);
	}
}

void Hero::setScale(float fScale)
{
	ActionSprite::setScale(fScale);
	if (_weapon)
	{
		_weapon->setScale(fScale);
	}
}

float Hero::attackDamage()
{
	if (_weapon)
	{
		return ActionSprite::getAttackDamage() + _weapon->_damageBonus;
	}
	return ActionSprite::getAttackDamage();
}

float Hero::attackTwoDamage()
{
	if (_weapon)
	{
		return _attackTwoDamage + _weapon->_damageBonus;
	}

	return _attackTwoDamage;
}

float Hero::attackThreeDamage()
{
	if (_weapon)
	{
		return _attackThreeDamage + _weapon->_damageBonus;
	}
	return _attackThreeDamage;
}