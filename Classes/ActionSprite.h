#ifndef _ActionState_
#define _ActionState_

#include "cocos2d.h"
#include "Defines.h"
#include "AnimationMember.h"

class ActionSprite;
//������
class ActionSpriteDelegate
{
public:
	virtual bool actionSpriteDidAttack(ActionSprite *actionSprite) = 0; //��ɫ������?
	virtual bool actionSpriteDidDie(ActionSprite *actionSprite) = 0;   //��ɫ�ǲ��ǹ���?
};

class ActionSprite : public cocos2d::CCSprite
{
public:
	ActionSprite();
	~ActionSprite();

	bool _didJumpAttack;   //�Ƿ���Ծ��������
	float _actionDelay;
	float _attackDelayTime;
	float _directionX;  //��ɫ���� 1.0f ������ -1.0f ������
	bool _isInvincible; //�Ƿ��޵�

	//Attachment
	//CC_SYNTHESIZE_RETAIN(cocos2d::CCSprite*, _shadow, Shadow); //Ӱ��
	cocos2d::CCSprite* _shadow;
	void setShadow(cocos2d::CCSprite *shadow)
	{
		_shadow = shadow;
	}
	cocos2d::CCSprite* getShadow() {return _shadow; }

	//actions
	//cocos2d::CCAction* _idleAction;
	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _idleAction, IdleAction);		//�ж���
	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _attackAction, AttackAction);  //��������
	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _walkAction, WalkAction);	    //���߶���
	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _hurtAction, HurtAction);
	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _knockedOutAction, KnockedOutAction);  //��������
	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _runAction, RunAction);
	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _recoverAction, RecoverAction);	    //������
	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _jumpRiseAction, JumpRiseAction);		//������
	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _jumpFallAction, JumpFallAction);		//��Ծ�����䶯��
	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _jumpLandAction, JumpLandAction);
	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _jumpAttackAction, JumpAttackAction);
	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _runAttackAction, RunAttackAction);
	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _dieAction, DieAction);

	//states
	//CC_SYNTHESIZE(ActionState, _actionState, ActionState);
	ActionState _actionState;				//��ɫ��ǰ������״̬

	//attributes
	CC_SYNTHESIZE(float, _walkSpeed, WalkSpeed);	   //�����ٶ�
	CC_SYNTHESIZE(float, _runSpeed, RunSpeed);		   //�ܲ��ٶ�
	CC_SYNTHESIZE(float, _hitPoints, HitPoints);	   //��ɫ��ǰ����ֵ
	CC_SYNTHESIZE(float, _attackDamage, AttackDamage); //��ɫ�ܵ����˺�
	CC_SYNTHESIZE(float, _jumpAttackDamage, JumpAttackDamage);
	CC_SYNTHESIZE(float, _runAttackDamage, RunAttackDamage);
	CC_SYNTHESIZE(float, _maxHitPoints, MaxHitPoints);  //��ɫ�������ֵ
	CC_SYNTHESIZE(float, _attackForce, AttackForce);    //��ɫ�ܵ��˺�ʱ�ĺ����,�ܵ��ĺ����Խ���ɫ����Խ��

	//movement
	CC_SYNTHESIZE(cocos2d::CCPoint, _velocity, Velocity);
	CC_SYNTHESIZE(float, _jumpVelocity, JumpVelocity);
	CC_SYNTHESIZE(float, _jumpHeight, JumpHeight); //��ɫ��ǰ��Ծ�ĸ߶�
	CC_SYNTHESIZE(cocos2d::CCPoint, _desiredPosition, DesiredPosition); //��ɫ���������λ��
	//CC_SYNTHESIZE(cocos2d::CCPoint, _groundPosition, GroundPosition);
	cocos2d::CCPoint _groundPosition; //�����λ��,���������λ��

	//measurements
	//��Ϊ����Ļ���ʵ���Ϻܴ�ԶԶ������ʵ��ռ�õ����(������ͼ�����˴�Ƭ�հף������ĵ����ڽ�ɫ�����ĵ�)��������������Ҫ�Լ����徫��Ĵ�С
	CC_SYNTHESIZE(float, _centerToSides, CenterToSides);  //���ý�ɫ��Χ��,��ɫ���ĵ㵽���ߵľ���
	CC_SYNTHESIZE(float, _centerToBottom, CenterToBottom);//���ý�ɫ��Χ��,��ɫ���ĵ㵽���µľ���

	//collisions
	std::vector<ContactPoint> _contactPoints; //�������ײ���Բ����
	std::vector<ContactPoint> _attackPoints;  //��������ײ���Բ   
	int _contactPointCount;  //��ײ���Բ��Ŀ
	int _attackPointCount;
	float _detectionRadius;  //���뾶��������˴˰뾶�ſ�ʼ��ײ���
	//cocos2d::CCRect feetCollisionRect;

	//animation
	//��ǰ׺prefix����ʼ֡����֡������ʱ���ɶ�������
	cocos2d::CCAnimation* animationWithPrefix(const char* prefix, int startFrameIdx, int frameCount, float delay);

	//action methods
	void idle();
	void moveWithDirection(cocos2d::CCPoint direction);
	void runWithDirection(cocos2d::CCPoint direction);
	void walkWithDirection(cocos2d::CCPoint direction);
	void attack();

	void setPosition(const cocos2d::CCPoint& pos);
	void setGroundPosition(cocos2d::CCPoint groundPosition);
	void jumpRiseWithDirection(cocos2d::CCPoint direction);
	void jumpRise();
	void jumpCutoff();
	void jumpFall();
	void jumpLand();
	void die();
	void recover();
	virtual void getUp();
	void reset();
	void jumpAttack();
	void runAttack();

	//�趨��ɫ�Զ���һ���ߵ���һ��
	void enterFrom(cocos2d::CCPoint origin, cocos2d::CCPoint destination);

	void setInvincibleState(float time);  //����һ��ʱ���ɫ�����޵�״̬
	void setVincible(float dt);			  //���ý�ɫΪ�����е�״̬
	/*
	
	void hurtWithDamage(float damage, float forceDirection, cocos2d::CCPoint direction);
	void knockoutWithDamage(float damageDirection, cocos2d::CCPoint direction);
	
	
	
	void enterFrom(cocos2d::CCPoint origin, cocos2d::CCPoint destination);
	
	*/

	void flipSpriteForVelocity(cocos2d::CCPoint velocity); //���ݽ�ɫ����ת����

	cocos2d::CCRect feetCollisionRect();				   //��ɫ�Ų����ص���ײ����

	//contact point methods
	//void modifyContactPointAtIndex(const int pointIndexOffset, const cocos2d::CCPoint offsetRadius, const float radius);

	virtual void update(float dt);

	//��ĳһĿ��target, ִ��ǰ׺Ϊprefix�Ķ���
	AnimationMember* animationMemberWithPrefix(const char* prefix, int startFrameIdx, int frameCount, float delay, cocos2d::CCSprite* target);

	ContactPoint contactPointWithOffset(const cocos2d::CCPoint offset, const float radius); //��ʼ����ײԲ

	void modifyPoint(ContactPoint *point, const cocos2d::CCPoint offset, const float radius); //�޸�ĳһ��ײԲ

	void modifyAttackPointAtIndex(const int pointIndex, const cocos2d::CCPoint offset, const float radius);

	void setActionState(ActionState actionState);
	virtual void setContactPointsForAction(ActionState actionState);
	void modifyContactPointAtIndex(int pointIndex, const cocos2d::CCPoint offset, const float radius); //���±�Ϊindex��������ײ�����

	void transformPoints();

	void hurtWithDamage(float damage, float force, cocos2d::CCPoint direction);
	void knockoutWithDamage(float damage, cocos2d::CCPoint direction);


	void cleanup();

	ActionSpriteDelegate* _delegate;
protected:
	
private:
};

#endif