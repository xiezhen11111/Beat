#ifndef _Hero_
#define _Hero_

#include "ActionSprite.h"
#include "Weapon.h"
#include "AnimateGroup.h"

class Hero : public ActionSprite, public WeaponDelegate
{
public:
	Hero();
	~Hero();

	CREATE_FUNC(Hero);
	bool init();

	void setContactPointsForAction(ActionState actionState); //根据当前状态设置碰撞点
	//重载CCSprite的setDisplayFrame函数，它是执行动画里调用的，用来判断攻击帧
	void setDisplayFrame(cocos2d::CCSpriteFrame *pNewFrame);//由于一套攻击动作可能真正的攻击动作是在某一帧，那么就不需要在其余帧检测（比如一套攻击动作有5帧，真正攻击帧在第4帧，所以应该在播放到第4帧的时候去检测，其余帧并不需要）

	void hurtWithDamage(float damage, float force, cocos2d::CCPoint direction);
	void knockoutWithDamage(float damage, cocos2d::CCPoint direction);
	void getUp();

	//重载这四个函数是为了让武器也保持与英雄同步
	void setPosition(const cocos2d::CCPoint& pos);
	void setScaleY(float fScaleY);
	void setScaleX(float fScaleX);
	void setScale(float fScale);

	float attackDamage();
	float attackTwoDamage();
	float attackThreeDamage();

	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _attackTwoAction, AttackTwoAction);
	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _attackThreeAction, AttackThreeAction);

	void update(float dt);

	void attack();

	void cleanup();

	void dropWeapon();
	bool pickUpWeapon(Weapon *weapon);
	void setWeapon(CCNode *pSender, void* data);

	void removeAllAnimationMembers();

	void weaponDidReachLimit(Weapon* weapon);

	float _attackTwoDamage;   //第二下伤害
	float _attackThreeDamage; //第三下伤害

	AnimateGroup *_attackGroup;
	AnimateGroup *_attackTwoGroup;
	AnimateGroup *_attackThreeGroup;
	AnimateGroup *_idleGroup;
	AnimateGroup *_walkGroup;

	Weapon *_weapon;
protected:
	void setLandingDisplayFrame();
private:
	/*当英雄不断的受击时他无法逃脱（一大群敌人在身边攻击的时候），当现实受击动画的时候，他不能移动和攻击但是可以继续的受击，这样显的无法继续了.
	解决方法是计算受击的数量让他倒下，当他连续十次被打的时候，让他倒下（倒下后不会接受受击了），这样给他时间去恢复*/
	float _hurtToLerance;//当被击中的时候，该值会减少，当为0时，英雄被击倒
	float _recoveryRate;//英雄恢复他的hurtTolerance的频率
	float _hurtLimit;//hurtTolerance的最大值

	float _attackTwoDelayTime;
	float _attackThreeDelayTime;
	float _chainTimer;

	
};
#endif