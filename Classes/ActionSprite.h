#ifndef _ActionState_
#define _ActionState_

#include "cocos2d.h"
#include "Defines.h"
#include "AnimationMember.h"

class ActionSprite;
//代理类
class ActionSpriteDelegate
{
public:
	virtual bool actionSpriteDidAttack(ActionSprite *actionSprite) = 0; //角色被攻击?
	virtual bool actionSpriteDidDie(ActionSprite *actionSprite) = 0;   //角色是不是挂了?
};

class ActionSprite : public cocos2d::CCSprite
{
public:
	ActionSprite();
	~ActionSprite();

	bool _didJumpAttack;   //是否跳跃攻击过了
	float _actionDelay;
	float _attackDelayTime;
	float _directionX;  //角色朝向 1.0f 面向右 -1.0f 面向左
	bool _isInvincible; //是否无敌

	//Attachment
	//CC_SYNTHESIZE_RETAIN(cocos2d::CCSprite*, _shadow, Shadow); //影子
	cocos2d::CCSprite* _shadow;
	void setShadow(cocos2d::CCSprite *shadow)
	{
		_shadow = shadow;
	}
	cocos2d::CCSprite* getShadow() {return _shadow; }

	//actions
	//cocos2d::CCAction* _idleAction;
	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _idleAction, IdleAction);		//闲动作
	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _attackAction, AttackAction);  //攻击动作
	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _walkAction, WalkAction);	    //行走动作
	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _hurtAction, HurtAction);
	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _knockedOutAction, KnockedOutAction);  //击倒动作
	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _runAction, RunAction);
	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _recoverAction, RecoverAction);	    //起身动作
	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _jumpRiseAction, JumpRiseAction);		//跳起动作
	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _jumpFallAction, JumpFallAction);		//跳跃中下落动作
	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _jumpLandAction, JumpLandAction);
	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _jumpAttackAction, JumpAttackAction);
	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _runAttackAction, RunAttackAction);
	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _dieAction, DieAction);

	//states
	//CC_SYNTHESIZE(ActionState, _actionState, ActionState);
	ActionState _actionState;				//角色当前所处的状态

	//attributes
	CC_SYNTHESIZE(float, _walkSpeed, WalkSpeed);	   //行走速度
	CC_SYNTHESIZE(float, _runSpeed, RunSpeed);		   //跑步速度
	CC_SYNTHESIZE(float, _hitPoints, HitPoints);	   //角色当前体力值
	CC_SYNTHESIZE(float, _attackDamage, AttackDamage); //角色受到的伤害
	CC_SYNTHESIZE(float, _jumpAttackDamage, JumpAttackDamage);
	CC_SYNTHESIZE(float, _runAttackDamage, RunAttackDamage);
	CC_SYNTHESIZE(float, _maxHitPoints, MaxHitPoints);  //角色最大体力值
	CC_SYNTHESIZE(float, _attackForce, AttackForce);    //角色受的伤害时的后冲力,受到的后冲力越大角色后退越多

	//movement
	CC_SYNTHESIZE(cocos2d::CCPoint, _velocity, Velocity);
	CC_SYNTHESIZE(float, _jumpVelocity, JumpVelocity);
	CC_SYNTHESIZE(float, _jumpHeight, JumpHeight); //角色当前跳跃的高度
	CC_SYNTHESIZE(cocos2d::CCPoint, _desiredPosition, DesiredPosition); //角色期望到达的位置
	//CC_SYNTHESIZE(cocos2d::CCPoint, _groundPosition, GroundPosition);
	cocos2d::CCPoint _groundPosition; //地面点位置,不算跳起的位置

	//measurements
	//因为精灵的画布实际上很大远远超过了实际占用的面积(美工画图留下了大片空白，但中心点是在角色正中心的)，所以这里我们要自己定义精灵的大小
	CC_SYNTHESIZE(float, _centerToSides, CenterToSides);  //设置角色包围框,角色中心点到两边的距离
	CC_SYNTHESIZE(float, _centerToBottom, CenterToBottom);//设置角色包围框,角色中心点到上下的距离

	//collisions
	std::vector<ContactPoint> _contactPoints; //挨打的碰撞检测圆数组
	std::vector<ContactPoint> _attackPoints;  //攻击的碰撞检测圆   
	int _contactPointCount;  //碰撞检测圆数目
	int _attackPointCount;
	float _detectionRadius;  //侦测半径，精灵过了此半径才开始碰撞检测
	//cocos2d::CCRect feetCollisionRect;

	//animation
	//用前缀prefix，开始帧，总帧数和延时生成动画动作
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

	//设定角色自动从一点走到另一点
	void enterFrom(cocos2d::CCPoint origin, cocos2d::CCPoint destination);

	void setInvincibleState(float time);  //设置一段时间角色处于无敌状态
	void setVincible(float dt);			  //设置角色为正常有敌状态
	/*
	
	void hurtWithDamage(float damage, float forceDirection, cocos2d::CCPoint direction);
	void knockoutWithDamage(float damageDirection, cocos2d::CCPoint direction);
	
	
	
	void enterFrom(cocos2d::CCPoint origin, cocos2d::CCPoint destination);
	
	*/

	void flipSpriteForVelocity(cocos2d::CCPoint velocity); //根据角色朝向翻转精灵

	cocos2d::CCRect feetCollisionRect();				   //角色脚部碰地的碰撞检测框

	//contact point methods
	//void modifyContactPointAtIndex(const int pointIndexOffset, const cocos2d::CCPoint offsetRadius, const float radius);

	virtual void update(float dt);

	//对某一目标target, 执行前缀为prefix的动画
	AnimationMember* animationMemberWithPrefix(const char* prefix, int startFrameIdx, int frameCount, float delay, cocos2d::CCSprite* target);

	ContactPoint contactPointWithOffset(const cocos2d::CCPoint offset, const float radius); //初始化碰撞圆

	void modifyPoint(ContactPoint *point, const cocos2d::CCPoint offset, const float radius); //修改某一碰撞圆

	void modifyAttackPointAtIndex(const int pointIndex, const cocos2d::CCPoint offset, const float radius);

	void setActionState(ActionState actionState);
	virtual void setContactPointsForAction(ActionState actionState);
	void modifyContactPointAtIndex(int pointIndex, const cocos2d::CCPoint offset, const float radius); //给下标为index的设置碰撞点参数

	void transformPoints();

	void hurtWithDamage(float damage, float force, cocos2d::CCPoint direction);
	void knockoutWithDamage(float damage, cocos2d::CCPoint direction);


	void cleanup();

	ActionSpriteDelegate* _delegate;
protected:
	
private:
};

#endif