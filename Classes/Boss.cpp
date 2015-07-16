#include "Boss.h"
#include "SimpleAudioEngine.h"


USING_NS_CC;
using namespace CocosDenshion;

Boss::Boss()
{

}

Boss::~Boss()
{

}

bool Boss::init()
{
	bool bRet = false;

	do 
	{
		CC_BREAK_IF(!CCSprite::initWithSpriteFrameName("boss_idle_00.png"));
		this->_shadow = CCSprite::createWithSpriteFrameName("shadow_character.png");
		this->_shadow->setOpacity(190);

		//idle animation
		CCAnimation *idleAnimation = this->animationWithPrefix("boss_idle", 0, 5, 1.f/10.f);
		CCRepeatForever* action = CCRepeatForever::create(CCAnimate::create(idleAnimation));
		this->setIdleAction(action);

		//attack animation
		CCAnimation *attackAnimation = this->animationWithPrefix("boss_attack", 0, 5, 1.f/8.f);
		CCSequence* seq = CCSequence::create(CCAnimate::create(attackAnimation), CCCallFunc::create(this, callfunc_selector(Boss::idle)), NULL);
		this->setAttackAction(seq);

		//walk animation
		CCAnimation* walkAnimation = this->animationWithPrefix("boss_walk", 0, 6, 1.f/8.f);
		this->setWalkAction(CCRepeatForever::create(CCAnimate::create(walkAnimation)));

		//hurt animation
		CCAnimation *hurtAnimation = this->animationWithPrefix("boss_hurt", 0 ,3, 1.f/12.f);
		CCSequence* seqHurt = CCSequence::create(CCAnimate::create(hurtAnimation), CCCallFunc::create(this, callfunc_selector(Boss::idle)), NULL);
		this->setHurtAction(seqHurt);

		//knocked out animation
		CCAnimation *knockedOutAnimation = this->animationWithPrefix("boss_knockout", 0, 4, 1.f/12.f);
		this->setKnockedOutAction(CCAnimate::create(knockedOutAnimation));

		//die action
		this->setDieAction(CCBlink::create(2.f, 10.f));

		//recover animation
		CCAnimation* recoverAnimation = this->animationWithPrefix("boss_getup", 0, 6, 1.f/12.f);
		CCSequence* seqRecover = CCSequence::create(CCAnimate::create(recoverAnimation), CCCallFunc::create(this, callfunc_selector(Boss::idle)), NULL);
		this->setRecoverAction(seqRecover);

		this->_walkSpeed = 60 * kPointFactor;
		this->_runSpeed = 120 * kPointFactor;
		this->_directionX = 1.f;
		this->setCenterToBottom(39.f * kPointFactor);
		this->setCenterToSides(42.f * kPointFactor);
		this->_detectionRadius = 90.f * kPointFactor;

		this->_contactPointCount = 4;
		int i=0;
		for (i=0; i<_contactPointCount; i++)
		{
			ContactPoint cp;
			_contactPoints.push_back(cp);
		}
		this->_attackPointCount = 1;
		for (i=0; i<_attackPointCount; i++)
		{
			ContactPoint cp;
			_attackPoints.push_back(cp);
		}
		this->modifyAttackPointAtIndex(0, ccp(65.f, 42.f), 23.7f);

		this->_maxHitPoints = 500.f;
		this->_hitPoints = this->_maxHitPoints;
		this->_attackDamage = 15.f;
		this->_attackForce = 2.f * kPointFactor;

		bRet = true;
	} while (0);
	return bRet;
}

void Boss::setContactPointsForAction(ActionState actionState)
{
	if (_actionState == kActionStateIdle)
	{
		this->modifyContactPointAtIndex(0, ccp(7.f, 36.f), 23.f);
		this->modifyContactPointAtIndex(1, ccp(-11.f, 17.f), 23.5f);
		this->modifyContactPointAtIndex(2, ccp(-2.f, -20.f), 23.f);
		this->modifyContactPointAtIndex(3, ccp(24.f, 9.f), 18.f);
	}
	else if (_actionState == kActionStateWalk)
	{
		this->modifyContactPointAtIndex(0, ccp(6.f, 41.f), 22.f);
		this->modifyContactPointAtIndex(1, ccp(-5.f, 16.f), 26.f);
		this->modifyContactPointAtIndex(2, ccp(1.f, -11.f), 17.f);
		this->modifyContactPointAtIndex(3, ccp(-13.f, -25.f), 10.f);
	}
	else if (_actionState == kActionStateAttack)
	{
		this->modifyContactPointAtIndex(0, ccp(20.f, 38.f), 22.f);
		this->modifyContactPointAtIndex(1, ccp(-8.f, 7.f), 27.3f);
		this->modifyContactPointAtIndex(2, ccp(49.f, 18.f), 19.f);
		this->modifyContactPointAtIndex(3, ccp(12.f, -8.f), 31.f);
		this->modifyAttackPointAtIndex(0, ccp(65.f, 42.f), 23.7f);
	}
}

void Boss::setDisplayFrame(cocos2d::CCSpriteFrame *pNewFrame)
{
	CCSprite::setDisplayFrame(pNewFrame);

	CCSpriteFrame *attackFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("boss_attack_01.png");
	CCSpriteFrame *attackFrame2 = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("boss_attack_02.png");

	if (pNewFrame==attackFrame || pNewFrame==attackFrame2)
	{
		this->_delegate->actionSpriteDidAttack(this);
	}
}

void Boss::hurtWithDamage(float damage, float force, cocos2d::CCPoint direction)
{
	if (this->_actionState>kActionStateNone && this->_actionState<kActionStateKnockedOut)
	{
		float ratio = this->_hitPoints / this->_maxHitPoints;

		if (ratio <= 0.7f)
		{
			this->stopAllActions();
			this->runAction(this->getHurtAction());
			this->_actionState = kActionStateHurt;
		}

		this->_hitPoints -= damage;
		this->setDesiredPosition(ccp(this->getPositionX()+direction.x*force, this->getPositionY()));
		//没血了，就被击倒
		if(this->_hitPoints <= 0)
			this->knockoutWithDamage(0, direction);
	}
}

void Boss::knockoutWithDamage(float damage, cocos2d::CCPoint direction)
{
	ActionSprite::knockoutWithDamage(damage, direction);

	if (this->_actionState == kActionStateKnockedOut && this->getHitPoints()<=0)
	{
		SimpleAudioEngine::sharedEngine()->playEffect("Sounds/enemydeath.caf");
	}
}