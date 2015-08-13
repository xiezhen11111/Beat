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
		//设置角色包围框
		this->setCenterToBottom(39.0f * kPointFactor); //中心点到底部的距离
		this->setCenterToSides(29.0f * kPointFactor);  //中心点到两边的距离

		//碰撞设定
		this->_detectionRadius = 100.f * kPointFactor;
		this->_attackPointCount = 3; //初始化攻击碰撞框为3个
		//推入一组空的CP，相当于初始化碰撞点列表
		int i = 0;
		for (i=0; i<_attackPointCount; i++)
		{
			ContactPoint cp;
			_attackPoints.push_back(cp);
		}
		this->_contactPointCount = 4; //初始化挨打碰撞框为4个
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
		//创建精灵idle动画
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

		//创建walk动画
		CCAnimation *walkAnimation = this->animationWithPrefix("hero_walk", 0, 8, 1.0f/12.0f);
		_walkGroup = AnimateGroup::actionWithAnimation(walkAnimation, 1);
		this->setWalkAction(CCRepeatForever::create(CCRepeatForever::create(_walkGroup)));
		this->_walkSpeed = 80 * kPointFactor;

		//创建run动画
		CCAnimation *runAnimation = this->animationWithPrefix("hero_run", 0, 8, 1.0f/12.0f);
		this->setRunAction(CCRepeatForever::create(CCAnimate::create(runAnimation)));
		this->_runSpeed = 160 * kPointFactor;

		//创建攻击动画
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

		//上跳动画
		CCArray* jumpRiseFrames = CCArray::createWithCapacity(2);
		jumpRiseFrames->addObject(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("hero_jump_05.png"));//05 00
		jumpRiseFrames->addObject(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("hero_jump_00.png"));
		CCAnimation* animation = CCAnimation::createWithSpriteFrames(jumpRiseFrames, 1.0f/12.0f);
		this->setJumpRiseAction(CCAnimate::create(animation));

		//下落动画
		CCAnimation* fallAnimation = this->animationWithPrefix("hero_jump", 1, 4, 1.0f/12.0f);
		CCAnimate* animate = CCAnimate::create(fallAnimation);
		this->setJumpFallAction(animate);

		//着陆动画
		CCSequence *seq = CCSequence::create(CCCallFunc::create(this, callfunc_selector(Hero::setLandingDisplayFrame)),CCDelayTime::create(1.0f/12.0f),
			CCCallFunc::create(this, callfunc_selector(Hero::idle)),NULL);
		this->setJumpLandAction(seq);

		//hurt 动画
		CCAnimation* hurtAnimation = this->animationWithPrefix("hero_hurt", 0, 3, 1.0f/12.0f);
		CCSequence* seq2 = CCSequence::create(CCAnimate::create(hurtAnimation), CCCallFunc::create(this, callfunc_selector(Hero::idle)), NULL);
		this->setHurtAction(seq2);

		//击倒动画
		CCAnimation* knockedOutAnimation = this->animationWithPrefix("hero_knockout", 0, 5, 1.0f/12.0f);
		this->setKnockedOutAction(CCAnimate::create(knockedOutAnimation));

		//死亡动画，闪烁几次即可
		this->setDieAction(CCBlink::create(2.0f, 10.0f));

		//起来动画
		CCAnimation* recoverAnimation = this->animationWithPrefix("hero_getup", 0, 6, 1.0f/12.0f);
		CCSequence* seq3 = CCSequence::create(CCAnimate::create(recoverAnimation), CCCallFunc::create(this, callfunc_selector(Hero::jumpLand)), NULL);
		this->setRecoverAction(seq3);

		//跳跃攻击动画
		CCAnimation *jumpAttackAnimation = this->animationWithPrefix("hero_jumpattack", 0, 5, 1.0f/10.0f);
		CCSequence* seqJumpAttack = CCSequence::create(CCAnimate::create(jumpAttackAnimation), CCCallFunc::create(this, callfunc_selector(Hero::jumpFall)), NULL);
		this->setJumpAttackAction(seqJumpAttack);

		//跑步攻击动画
		CCAnimation *runAttackAnimation = this->animationWithPrefix("hero_runattack", 0, 6, 1.0f/10.0f);
		CCSequence *runAttack = CCSequence::create(CCAnimate::create(runAttackAnimation), CCCallFunc::create(this, callfunc_selector(Hero::idle)), NULL);
		this->setRunAttackAction(runAttack);

		//两下攻击动画
		CCAnimation *attackTwoAnimation = this->animationWithPrefix("hero_attack_01", 0, 3, 1.f/12.f);
		_attackTwoGroup = AnimateGroup::actionWithAnimation(attackTwoAnimation, 1);
		CCSequence* seqAtkTwo = CCSequence::create(_attackTwoGroup, CCCallFunc::create(this, callfunc_selector(Hero::idle)), NULL);
		this->setAttackTwoAction(seqAtkTwo);

		//三下攻击动画
		CCAnimation *attackThreeAnimation = this->animationWithPrefix("hero_attack_02", 0, 5, 1.f/10.f);
		_attackThreeGroup = AnimateGroup::actionWithAnimation(attackThreeAnimation, 1);
		CCSequence* seqAtkThree = CCSequence::create(_attackThreeGroup, CCCallFunc::create(this, callfunc_selector(Hero::idle)), NULL);
		this->setAttackThreeAction(seqAtkThree);

		//连击的相关设置
		
		//1.设置了二，三连击的攻击等到时间
		_attackTwoDelayTime = kOneAtkTime;
		_attackThreeDelayTime = 0.45f;
		
		//2.连击的间隔时间，该时间内再次攻击的话会触发连击
		_chainTimer = 0;

		//3.二三连击的攻击力
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
	float dir = this->_directionX; //偏移要考虑角色朝向
	switch (actionState)
	{
	case kActionStateIdle:
		this->modifyContactPointAtIndex(0, ccp(3.0f, 23.0f), 19.0f);
		this->modifyContactPointAtIndex(1, ccp(17.0f, 10.0f), 10.0f);
		this->modifyContactPointAtIndex(2, CCPointZero, 19.0f);
		this->modifyContactPointAtIndex(3, ccp(0.0f, -21.0f), 20.0f);

		this->_attackPointCount = 0;   // idle状态时没有攻击判定
		break;
	case kActionStateWalk:
		this->modifyContactPointAtIndex(0, ccp(8.0f, 23.0f), 19.0f);
		this->modifyContactPointAtIndex(1, ccp(12.0f, 4.0f), 4.0f);
		this->modifyContactPointAtIndex(2, CCPointZero, 10.0f);
		this->modifyContactPointAtIndex(3, ccp(0.0f, -21.0f), 20.0f);

		this->_attackPointCount = 0;   // idle状态时没有攻击判定

		break;
	case kActionStateAttack:
		this->modifyContactPointAtIndex(0, ccp(15.0f, 23.0f), 19.0f);
		this->modifyContactPointAtIndex(1, ccp(24.5f, 4.0f), 6.0f);
		this->modifyContactPointAtIndex(2, CCPointZero, 16.0f);
		this->modifyContactPointAtIndex(3, ccp(0.0f, -21.0f), 20.0f);

		this->modifyAttackPointAtIndex(0, ccp(41.0f, 3.0f), 10.0f);
		this->modifyAttackPointAtIndex(1, ccp(41.0f, 3.0f), 10.0f);
		this->modifyAttackPointAtIndex(2, ccp(41.0f, 3.0f), 10.0f);

		this->_attackPointCount = 3;   // 有3个攻击判定
		break;
	case kActionStateAttackTwo:
		this->modifyAttackPointAtIndex(0, ccp(51.6f, 2.4f), 13.0f);
		this->modifyAttackPointAtIndex(1, ccp(51.6f, 2.4f), 13.0f);
		this->modifyAttackPointAtIndex(2, ccp(51.6f, 2.4f), 13.0f);

		this->_attackPointCount = 3;   // 有3个攻击判定
		break;
	case kActionStateAttackThree:
		this->modifyAttackPointAtIndex(0, ccp(61.8f, 6.2f), 22.0f);
		this->modifyAttackPointAtIndex(1, ccp(61.8f, 6.2f), 22.0f);
		this->modifyAttackPointAtIndex(2, ccp(61.8f, 6.2f), 22.0f);

		this->_attackPointCount = 3;   // 有3个攻击判定
		break;
	case kActionStateRunAttack:
		this->modifyAttackPointAtIndex(0, ccp(31.2f, -8.8f), 10.0f);
		this->modifyAttackPointAtIndex(1, ccp(31.2f, -8.8f), 10.0f);
		this->modifyAttackPointAtIndex(2, ccp(31.2f, -8.8f), 10.0f);

		this->_attackPointCount = 3;   // 有3个攻击判定
		break;
	case kActionStateJumpAttack:
		this->modifyAttackPointAtIndex(2, ccp(70.0f, -55.0f), 8.0f);
		this->modifyAttackPointAtIndex(1, ccp(55.0f, -42.0f), 12.0f);
		this->modifyAttackPointAtIndex(0, ccp(34.0f, -25.0f), 17.0f);

		this->_attackPointCount = 3;   // 有3个攻击判定
		break;
	default:
		break;
	}
	
}

void Hero::setDisplayFrame(cocos2d::CCSpriteFrame *pNewFrame)
{
	CCSprite::setDisplayFrame(pNewFrame);

	//检测当前播放的是不是攻击帧，以下是攻击动作的中真正的攻击帧
	CCSpriteFrame *attackFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("hero_attack_00_01.png");
	CCSpriteFrame *runAttackFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("hero_runattack_02.png");
	CCSpriteFrame *runAttackFrame2 = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("hero_runattack_03.png");
	CCSpriteFrame *jumpAttackFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("hero_jumpattack_02.png");

	//new frames 连击的攻击帧
	CCSpriteFrame *attackFrame2 = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("hero_attack_01_01.png");
	CCSpriteFrame *attackFrame3 = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("hero_attack_02_02.png");

	if (pNewFrame == attackFrame || pNewFrame==attackFrame2)
	{
		if(this->_delegate->actionSpriteDidAttack(this)) //返回true表示角色攻击到机器人了
		{
			//攻击到了，设置连击的触发时间
			_chainTimer = 0.3f;   //如果是第一击或第二击，要重设下下一击有效时间

			if (_weapon)
			{
				_weapon->used();
			}
		}
	}
	else if (pNewFrame == attackFrame3)
	{
		//this->_delegate->actionSpriteDidAttack(this);
		if (this->_delegate->actionSpriteDidAttack(this)) //成功触发了攻击行为
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
		this->_delegate->actionSpriteDidAttack(this); //代理模式处理攻击行为
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
			//被击倒了
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

	//连击的有效时间更新（在大于0时间内会触发连击，否则视为普通攻击）
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
		//理论上这里执行无此，这样的话只有连续多次的击中英雄才会被击倒
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
	连击检测原理：连击的时间间隔为0.3s，当敌人第一次被攻击的时候，会设置下一个连击的间隔为0.3s，如果下一个攻击在0.3s内发生了就会触发二次连击，否则视为普通的连击，
	同理第三击也是这样，如果形成三连击将获得更大的攻击力（需要注意的是_actionDelay（攻击的等待时间）的值要小于_chainTimer，否则按了也无效）
	*/

	//根据当前的攻击状态及是否在连击的有效时间攻击，来判断是否触发连击及连击的阶段

	//判断当前的攻击状态，处于连击中第几个，_chainTimer是连击的有效时间，过了就不会触发
	if (this->_actionState == kActionStateAttack && _chainTimer>0)
	{
		//触发第二次连击
		_chainTimer = 0;//会在setDisplayFrame中检测到后设置其值（0.3）
		this->stopAllActions();
		this->runAction(_attackTwoAction);
		this->setActionState(kActionStateAttackTwo);
		_actionDelay = _attackTwoDelayTime;//更新攻击的等待时间（不是0的时候，攻击无效）
	}
	else if (this->_actionState == kActionStateAttackTwo && _chainTimer>0)
	{
		//触发第三次连击
		_chainTimer = 0;
		this->stopAllActions();
		this->runAction(_attackThreeAction);
		this->setActionState(kActionStateAttackThree);
		_actionDelay = _attackThreeDelayTime;
	}
	else//否则，正常的攻击
		ActionSprite::attack();
}

void Hero::getUp()
{
	ActionSprite::getUp();
	//设置一定时间无敌
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
	//把武器从动画列表里移除
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