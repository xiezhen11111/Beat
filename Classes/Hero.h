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

	void setContactPointsForAction(ActionState actionState); //���ݵ�ǰ״̬������ײ��
	//����CCSprite��setDisplayFrame����������ִ�ж�������õģ������жϹ���֡
	void setDisplayFrame(cocos2d::CCSpriteFrame *pNewFrame);

	void hurtWithDamage(float damage, float force, cocos2d::CCPoint direction);
	void knockoutWithDamage(float damage, cocos2d::CCPoint direction);
	void getUp();

	//����������������Ϊ��������Ҳ������Ӣ��ͬ��
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

	float _attackTwoDamage;   //�ڶ����˺�
	float _attackThreeDamage; //�������˺�

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