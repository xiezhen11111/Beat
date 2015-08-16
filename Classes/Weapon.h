#pragma once

#ifndef _Weapon_
#define _Weapon_

#include "cocos2d.h"
#include "Defines.h"
#include "AnimationMember.h"

class Weapon;
//代理类
class WeaponDelegate
{
public:
	virtual void weaponDidReachLimit(Weapon* weapon) = 0;
};

/**
 * 
 * 武器父类
 * 
*/	
class Weapon : public cocos2d::CCSprite
{
public:

	Weapon();
	~Weapon();

	//CREATE_FUNC(Weapon);

	CC_SYNTHESIZE(WeaponDelegate*, _delegate, Delegate);

	//动画部分有子类实现（每个武器都是不同的，所以应该要子类来实现）
	CCSprite* _shadow;
	AnimationMember* _attack;
	AnimationMember* _attackTwo;
	AnimationMember* _attackThree;
	AnimationMember* _idle;
	AnimationMember* _walk;
	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _droppedAction, DroppedAction);
	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _destroyedAction, DestroyedAction);

	float _damageBonus;
	int _limit;
	float _jumpVelocity;
	cocos2d::CCPoint _velocity;
	float _jumpHeight;
	cocos2d::CCPoint _groundPosition;
	WeaponState _weaponState;
	float _centerToBottom;
	float _detectionRadius;

	cocos2d::CCAnimation* animationWithPrefix(const char* prefix, int startFrameIdx, int frameCount, float delay);
	AnimationMember* animationMemberWithPrefix(const char* prefix, int startFrameIdx, int frameCount, float delay, cocos2d::CCSprite* target);
	void used();
	void pickedUp();
	void dropped(float height, cocos2d::CCPoint dest);
	void reset();
	void setVisible(bool visible);
	void setGroundPosition(cocos2d::CCPoint groundPosition);
	void droppedFrom(float height, cocos2d::CCPoint destination);//武器掉落，具体的位置计算参见295

	void cleanup();

	void update(float dt);
protected:
private:
};
#endif