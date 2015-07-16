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
	void setDisplayFrame(cocos2d::CCSpriteFrame *pNewFrame);

	void hurtWithDamage(float damage, float force, cocos2d::CCPoint direction);
	void knockoutWithDamage(float damage, cocos2d::CCPoint direction);
	void getUp();

	//重载这三个函数是为了让武器也保持与英雄同步
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
	float _hurtToLerance;
	float _recoveryRate;
	float _hurtLimit;

	float _attackTwoDelayTime;
	float _attackThreeDelayTime;
	float _chainTimer;

	
};
#endif