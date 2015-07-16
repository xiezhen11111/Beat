#include "Weapon.h"
USING_NS_CC;

Weapon::Weapon()
{
	_delegate = NULL;
	_shadow = NULL;
	_attack = NULL;
	_attackTwo = NULL;
	_attackThree = NULL;
	_idle = NULL;
	_walk = NULL;
	_droppedAction = NULL;
	_destroyedAction = NULL;

}

Weapon::~Weapon()
{

}

CCAnimation* Weapon::animationWithPrefix(const char* prefix, int startFrameIdx, int frameCount, float delay)
{
	int idxCount = frameCount + startFrameIdx;
	CCArray *frames = CCArray::createWithCapacity(frameCount);
	int i;
	//CCSpriteFrame *frame;
	for (i=startFrameIdx; i<idxCount; i++)
	{
		CCString* str = CCString::createWithFormat("%s_%02d.png",prefix, i);
		CCSpriteFrame *frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(str->getCString());
		frames->addObject(frame);
	}

	return CCAnimation::createWithSpriteFrames(frames, delay);
}

AnimationMember* Weapon::animationMemberWithPrefix(const char* prefix, int startFrameIdx, int frameCount, float delay, cocos2d::CCSprite* target)
{
	CCAnimation *animation = this->animationWithPrefix(prefix, startFrameIdx, frameCount, delay);
	return AnimationMember::memberWithAnimation(animation, target);
}

void Weapon::reset()
{
	this->setVisible(false);
	_shadow->setVisible(false);
	_weaponState = kWeaponStateNone;
	this->_velocity = CCPointZero;
	this->_jumpVelocity = 0.f; 
}

void Weapon::setVisible(bool visible)
{
	CCSprite::setVisible(visible);
	this->_shadow->setVisible(visible);
}

void Weapon::setGroundPosition(cocos2d::CCPoint groundPosition)
{
	_groundPosition = groundPosition;
	_shadow->setPosition(ccp(groundPosition.x ,groundPosition.y - _centerToBottom));
}

void Weapon::used()
{
	this->_limit--;  //武器的使用数减少

	if (_limit<=0)   //武器可用次数为0时，武器掉落
	{
		_delegate->weaponDidReachLimit(this);
		this->_weaponState = kWeaponStateDestroyed;
		this->runAction(_destroyedAction);
	}
}

void Weapon::pickedUp()
{
	this->_weaponState = kWeaponStateEquipped;
	this->_shadow->setVisible(false);
}

void Weapon::droppedFrom(float height, CCPoint destination)
{
	_jumpVelocity = kJumpCutoff;
	_jumpHeight = height;
	this->setGroundPosition(destination);
	this->_weaponState = kWeaponStateUnequipped;
	this->_shadow->setVisible(true);
	this->runAction(_droppedAction);
}

void Weapon::update(float dt)
{
	if (_weaponState > kWeaponStateEquipped) //武器不是装备状态，则武器掉落地上
	{
		this->setGroundPosition(ccpAdd(this->_groundPosition, ccpMult(_velocity, dt)));
		_jumpVelocity -= kGravity * dt;
		_jumpHeight += _jumpVelocity * dt;

		if (_jumpHeight<0)
		{
			_velocity = CCPointZero;
			_jumpVelocity = 0;
			_jumpHeight = 0;
		}
	}
}

void Weapon::cleanup()
{
	_delegate = NULL;
	_shadow = NULL;
	_attack = NULL;
	_attackTwo = NULL;
	_attackThree = NULL;
	_idle = NULL;
	_walk = NULL;
	_droppedAction = NULL;
	_destroyedAction = NULL;
	CCSprite::cleanup();
}