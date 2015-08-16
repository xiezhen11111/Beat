#include "Gauntlets.h"
#include "Defines.h"
USING_NS_CC;

Gauntlets::Gauntlets()
{

}

Gauntlets::~Gauntlets()
{

}

bool Gauntlets::init()
{
	bool bRet = false;

	do 
	{
		CC_BREAK_IF(!CCSprite::initWithSpriteFrameName("weapon_unequipped.png"));

		//创建攻击动画,第一个攻击比较特殊，这里帧数原素材不够自己拼的
		CCArray *frames = CCArray::createWithCapacity(5);
		CCSpriteFrame *frame;
		for (int i=0; i<3; i++)
		{
			frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(CCString::createWithFormat("weapon_attack_00_%02d.png", i)->getCString());
			frames->addObject(frame);
		}
		for(int i=1; i>=0; i--)
		{
			frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(CCString::createWithFormat("weapon_attack_00_%02d.png", i)->getCString());
			frames->addObject(frame);
		}
		CCAnimation *attackAnimation = CCAnimation::createWithSpriteFrames(frames, 1.0f/15.0f);
		this->_attack = AnimationMember::memberWithAnimation(attackAnimation, this);

		this->_attackTwo = this->animationMemberWithPrefix("weapon_attack_01", 0, 3, 1.f/12.f, this);

		this->_attackThree = this->animationMemberWithPrefix("weapon_attack_02", 0, 3, 1.f/10.f, this);
		this->_idle = this->animationMemberWithPrefix("weapon_idle", 0, 6, 1.f/12.f, this);
		this->_walk = this->animationMemberWithPrefix("weapon_walk", 0, 8, 1.f/12.f, this);


		//武器掉落的动画
		CCSpriteFrame *dropFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("weapon_unequipped.png");
		CCArray* dropFrames = CCArray::createWithCapacity(2);
		dropFrames->addObject(dropFrame);
		dropFrames->addObject(dropFrame);
		CCAnimation *dropAnimation = CCAnimation::createWithSpriteFrames(dropFrames, 1.f/12.f);
		CCAnimate* amte = CCAnimate::create(dropAnimation);
		this->setDroppedAction(amte);

		this->setDestroyedAction(CCSequence::create(CCBlink::create(2.f, 5), CCCallFunc::create(this, callfunc_selector(Gauntlets::reset)), NULL));
		this->_damageBonus = 20.f;
		this->_centerToBottom = 5.f * kPointFactor;

		this->_shadow = CCSprite::createWithSpriteFrameName("shadow_weapon.png");
		this->_shadow->setOpacity(190);
		this->_detectionRadius = 10.f * kPointFactor;

		this->reset();
		bRet = true;
	} while (0);
	return bRet;
}

void Gauntlets::reset()
{
	Weapon::reset();
	this->_limit = 8;
}

void Gauntlets::cleanup()
{
	this->_attack = NULL;
	this->_attackTwo = NULL;
	this->_attackThree = NULL;
	this->_idle = NULL;
	this->_walk = NULL;
	Weapon::cleanup();
}
