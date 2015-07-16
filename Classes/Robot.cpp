#include "Robot.h"
#include "AnimateGroup.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace std;
using namespace CocosDenshion;

Robot::Robot()
{
	_belt = NULL;
	_smoke = NULL;
}

Robot::~Robot()
{

}

bool Robot::init()
{
	bool bRet = false;

	do 
	{
	  CC_BREAK_IF(!ActionSprite::initWithSpriteFrameName("robot_base_idle_00.png"));

	  this->_belt = CCSprite::createWithSpriteFrameName("robot_belt_idle_00.png");
	  this->_smoke = CCSprite::createWithSpriteFrameName("robot_smoke_idle_00.png");

	  //阴影
	  CCSprite* shadow = CCSprite::createWithSpriteFrameName("shadow_character.png");
	  shadow->setOpacity(190);
	  this->_shadow = shadow;
	  //this->setShadow(shadow);

	  //设置动画组
	  //idle动画
	  AnimateGroup *idleAnimationGroup = this->animateGroupWithActionWord("idle", 5, 1.0f/12.0f);
	  this->setIdleAction(CCRepeatForever::create(idleAnimationGroup));
	  //attack动画组
	  AnimateGroup *attackAnimationGroup = this->animateGroupWithActionWord("attack", 5, 1.0f/12.0f);
	  this->setAttackAction(CCSequence::create(attackAnimationGroup, CCCallFunc::create(this, callfunc_selector(Robot::idle)), NULL));
	  //WALK动画组
	  AnimateGroup* walkAnimationGroup = this->animateGroupWithActionWord("walk", 6, 1.0f/12.0f);
	  this->setWalkAction(CCRepeatForever::create(walkAnimationGroup));

	  //Hurt 动画
	  AnimateGroup *hurtAnimationGroup = this->animateGroupWithActionWord("hurt", 3, 1.0f/12.0f);
	  CCSequence* seq = CCSequence::create(hurtAnimationGroup, CCCallFunc::create(this, callfunc_selector(Robot::idle)), NULL);
	  this->setHurtAction(seq);

	  //击倒动画
	  AnimateGroup* knockoutAnimation = this->animateGroupWithActionWord("knockout", 5, 1.0f/12.0f);
	  this->setKnockedOutAction(knockoutAnimation);

	  //死亡动画
	  this->setDieAction(CCBlink::create(2.0f, 10.0f));

	  //起来动画
	  AnimateGroup* recoverAnimation = this->animateGroupWithActionWord("getup", 6, 1.0f/12.0f);
	  CCSequence *seq2 = CCSequence::create(recoverAnimation, CCCallFunc::create(this, callfunc_selector(Robot::idle)), NULL);
	  this->setRecoverAction(seq2);

	  this->setWalkSpeed(80*kPointFactor);
	  this->setRunSpeed(160*kPointFactor);
	  this->_directionX = 1.0;
	  this->_centerToBottom = 39.0 * kPointFactor;
	  this->_centerToSides = 29.0 * kPointFactor;

	  this->_detectionRadius = 50.0 * kPointFactor;
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
	  this->modifyAttackPointAtIndex(0, ccp(45.f, 6.5f), 10.f); //初始化一个攻击判定框

	  this->_maxHitPoints = 100.0f;  //角色最大HP
	  this->_hitPoints = this->_maxHitPoints;  //角色当前HP
	  this->setAttackDamage(4);
	  this->_attackForce = 0.f;//2.0 * kPointFactor;

	 // this->setActionState(kActionStateIdle);

	  bRet  = true;
	} while (0);

	return bRet;
}

void Robot::setPosition(cocos2d::CCPoint position)
{
	ActionSprite::setPosition(position);
	_belt->setPosition(position);
	_smoke->setPosition(position);
	
	//this->getShadow()->setPosition(ccp(_groundPosition.x, _groundPosition.y - _centerToBottom));
}

void Robot::setScaleX(float fScaleX)
{
	ActionSprite::setScaleX(fScaleX);
	_belt->setScaleX(fScaleX);
	_smoke->setScaleX(fScaleX);
}

void Robot::setScaleY(float fScaleY)
{
	ActionSprite::setScaleY(fScaleY);
	_belt->setScaleY(fScaleY);
	_smoke->setScaleY(fScaleY);
}

void Robot::setScale(float fScale)
{
	ActionSprite::setScale(fScale);
	_belt->setScale(fScale);
	_smoke->setScale(fScale);
}

void Robot::setVisible(bool bVisible)
{
	ActionSprite::setVisible(bVisible);
	_belt->setVisible(bVisible);
	_smoke->setVisible(bVisible);
}

AnimateGroup* Robot::animateGroupWithActionWord(const char* actionKeyWord, int frameCount, float delay)
{
	CCAnimation* baseAnimation = this->animationWithPrefix(CCString::createWithFormat("robot_base_%s",actionKeyWord)->getCString(), 0, frameCount,delay);

	//腰带
	AnimationMember *beltMember = this->animationMemberWithPrefix(CCString::createWithFormat("robot_belt_%s", actionKeyWord)->getCString(), 0, frameCount, delay, _belt);
	//头上的烟
	AnimationMember *smokeMember = this->animationMemberWithPrefix(CCString::createWithFormat("robot_smoke_%s", actionKeyWord)->getCString(), 0, frameCount, delay, _smoke);

	CCArray *animationMembers = CCArray::create();
	animationMembers->addObject(beltMember);
	animationMembers->addObject(smokeMember);

	return AnimateGroup::actionWithAnimation(baseAnimation, animationMembers);
}

void Robot::setColorSet(ColorSet colorSet)
{
	_colorSet = colorSet;
	if (colorSet == kColorLess)
	{
		this->setColor(ccWHITE);
		_belt->setColor(ccWHITE);
		_smoke->setColor(ccWHITE);
		this->_maxHitPoints = 50.f;
		this->_attackDamage = 2.f;
	}
	else if (colorSet == kColorCopper) //青铜圣斗士
	{
		this->setColor(ccc3(255, 193, 158));
		_belt->setColor(ccc3(99, 162, 255));
		_smoke->setColor(ccc3(220, 219, 182));

		this->_maxHitPoints = 100.f;
		this->_attackDamage = 4.f;
	}
	else if (colorSet == kColorSilver) //白银圣斗士
	{
		this->setColor(ccWHITE);
		_belt->setColor(ccc3(99, 255, 128));
		_smoke->setColor(ccc3(128, 128, 128));

		this->_maxHitPoints = 125.f;
		this->_attackDamage = 5.f;
	}
	else if (colorSet == kColorGold) //黄金圣斗士
	{
		this->setColor(ccc3(233, 177, 0));
		_belt->setColor(ccc3(109, 40, 25));
		_smoke->setColor(ccc3(222, 129, 82));

		this->_maxHitPoints = 150.f;
		this->_attackDamage = 6.f;
	}
	else if (colorSet == kColorRandom)
	{
		this->setColor(ccc3(random_range(0, 255), random_range(0, 255), random_range(0, 255)));
		this->setColor(ccc3(random_range(0, 255), random_range(0, 255), random_range(0, 255)));
		this->setColor(ccc3(random_range(0, 255), random_range(0, 255), random_range(0, 255)));

		this->_maxHitPoints = random_range(100, 150);
		this->_attackDamage = random_range(2, 4);
	}

	//this->_maxHitPoints = 10.f; //作弊专用
	//this->_hitPoints = this->_maxHitPoints;
}

void Robot::setContactPointsForAction(ActionState actionState)
{			
	switch (actionState)
	{
	case kActionStateIdle:
		this->modifyContactPointAtIndex(0, ccp(1.7f, 19.5f), 20.0f);
		this->modifyContactPointAtIndex(1, ccp(-15.5f, 3.5f), 16.0f);
		this->modifyContactPointAtIndex(2, ccp(17.f, 2.1f), 14.0f);
		this->modifyContactPointAtIndex(3, ccp(-0.8f, -18.5f), 19.0f);

		this->modifyAttackPointAtIndex(0, ccp(45.0f, 6.5f), 10.0f);
		break;
	case kActionStateWalk:
		this->modifyContactPointAtIndex(0, ccp(8.0f, 23.0f), 19.0f);
		this->modifyContactPointAtIndex(1, ccp(12.0f, 4.0f), 4.0f);
		this->modifyContactPointAtIndex(2, CCPointZero, 10.0f);
		this->modifyContactPointAtIndex(3, ccp(0.0f, -21.0f), 20.0f);

		this->modifyAttackPointAtIndex(0, ccp(45.0f, 6.5f), 10.0f);
		break;
	case kActionStateAttack:
		this->modifyContactPointAtIndex(0, ccp(15.0f, 23.0f), 19.0f);
		this->modifyContactPointAtIndex(1, ccp(24.5f, 4.0f), 6.0f);
		this->modifyContactPointAtIndex(2, CCPointZero, 16.0f);
		this->modifyContactPointAtIndex(3, ccp(0.0f, -21.0f), 20.0f);

		this->modifyAttackPointAtIndex(0, ccp(45.0f, 6.5f), 10.0f);
		break;
	}
}

void Robot::setDisplayFrame(cocos2d::CCSpriteFrame *pNewFrame)
{
	CCSprite::setDisplayFrame(pNewFrame);

	CCSpriteFrame* attackFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("robot_base_attack_03.png");
	if (pNewFrame == attackFrame)
	{
		this->_delegate->actionSpriteDidAttack(this);
	}
}

void Robot::reset()
{
	this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("robot_base_idle_00.png"));
	this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("robot_belt_idle_00.png"));
	this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("robot_smoke_idle_00.png"));

	ActionSprite::reset();
}

void Robot::knockoutWithDamage(float damage, cocos2d::CCPoint direction)
{
	ActionSprite::knockoutWithDamage(damage, direction);
	if (this->_actionState == kActionStateKnockedOut && this->getHitPoints()<=0)
	{
		SimpleAudioEngine::sharedEngine()->playEffect("Sounds/enemydeath.caf");
	}
}