#include "ActionSprite.h"
#include "GameScene.h"
USING_NS_CC;
using namespace std;

ActionSprite::ActionSprite()
{
	_directionX = 1.0f;
	_actionState = kActionStateNone;
	_isInvincible = false;
	_idleAction = NULL;
	_walkAction = NULL;
	_runAction = NULL;
	_attackAction = NULL;
	_jumpRiseAction = NULL;
	_jumpFallAction = NULL;
	_jumpLandAction = NULL;
	_hurtAction = NULL;
	_knockedOutAction = NULL;
	_dieAction = NULL;
	_recoverAction = NULL;
	_jumpAttackAction = NULL;
	_runAttackAction = NULL;

	_shadow = NULL;
	_delegate = NULL;

	//_contactPoints = NULL;
	_contactPointCount = 0;
	//_attackPoints = NULL;
	_attackPointCount = 0;
	_detectionRadius = 0.f;

	_jumpHeight = 0;

	_desiredPosition = ccp(100,100);
}

ActionSprite::~ActionSprite()
{
	_contactPoints.clear();
	_attackPoints.clear();
}

void ActionSprite::idle()
{
	if (_actionState!=kActionStateIdle)
	{
		this->stopAllActions();
		this->runAction(this->getIdleAction());
		_velocity = CCPointZero;
		//this->_actionState = kActionStateIdle;
		this->setActionState(kActionStateIdle);
		_actionDelay = 0.0;
	}
}

CCAnimation* ActionSprite::animationWithPrefix(const char* prefix, int startFrameIdx, int frameCount, float delay)
{
	int idxCount = frameCount + startFrameIdx; //总帧数
	CCArray *frames = CCArray::createWithCapacity(frameCount);
	CCSpriteFrame *frame;
	for (int i=startFrameIdx; i<idxCount; i++)
	{
		frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(CCString::createWithFormat("%s_%02d.png", prefix, i)->getCString());
		frames->addObject(frame);
	}

	return CCAnimation::createWithSpriteFrames(frames, delay);
}

void ActionSprite::walkWithDirection(CCPoint direction)
{
	if(_actionState == kActionStateIdle || _actionState == kActionStateRun) //当前不在行走状态,要播放行走动画
	{
		this->stopAllActions();
		this->runAction(this->getWalkAction());
		//this->_actionState = kActionStateWalk;
		this->setActionState(kActionStateWalk);
		this->moveWithDirection(direction);
	}
	else if(_actionState == kActionStateWalk) //如果当前已经是行走状态,不必再播放动画了
	{
		this->moveWithDirection(direction); //仅仅移动角色
	}
}

void ActionSprite::moveWithDirection(CCPoint direction)
{
	if (_actionState == kActionStateWalk) //根据行走，还是跑设置不同的速度
	{
		_velocity = ccp(direction.x * _walkSpeed, direction.y * _walkSpeed);
		//根据速度方向的不同设置角色朝向
		this->flipSpriteForVelocity(_velocity);
	}
	else if (_actionState == kActionStateRun)
	{
		_velocity = ccp(direction.x * _runSpeed, direction.y * _walkSpeed);
		this->flipSpriteForVelocity(_velocity);
	}
	else if (_actionState == kActionStateIdle) //如果当前是idle状态，要切换播放行走动画
	{
		this->walkWithDirection(direction);
	}
}

void ActionSprite::runWithDirection(CCPoint direction)
{
	if (_actionState == kActionStateIdle || _actionState == kActionStateWalk)
	{
		this->stopAllActions();
		this->runAction(_runAction);
		//this->_actionState = kActionStateRun;
		this->setActionState(kActionStateRun);
		this->moveWithDirection(direction);
	}
}

void ActionSprite::flipSpriteForVelocity(CCPoint velocity)
{
	if(velocity.x>0)
		this->_directionX = 1.0f;
	else if(velocity.x < 0)
		this->_directionX = -1.0f;

	this->setScaleX(_directionX * kScaleFactor);
}

CCRect ActionSprite::feetCollisionRect()
{
	CCRect feetRect = CCRectMake(_desiredPosition.x - _centerToSides, _desiredPosition.y -_centerToBottom, 
		_centerToSides*2, 7.0f * kPointFactor);
	return feetRect;
}

void ActionSprite::setPosition(const CCPoint& pos)
{
	CCSprite::setPosition(pos);
	this->transformPoints();   //加上这一句是保证角色在移动时判定框也要跟着角色移动
}

void ActionSprite::setGroundPosition(cocos2d::CCPoint groundPosition)
{
	_groundPosition = groundPosition;
	_shadow->setPosition(ccp(groundPosition.x, groundPosition.y - _centerToBottom));
}

void ActionSprite::jumpRiseWithDirection(cocos2d::CCPoint direction)
{
	if (_actionState == kActionStateIdle)
	{
		this->jumpRise();
	}
	else if (_actionState == kActionStateWalk || _actionState == kActionStateJumpLand)
	{
		_velocity = ccp(direction.x * _walkSpeed, direction.y * _walkSpeed);
		this->flipSpriteForVelocity(_velocity);
		this->jumpRise();
	}
	else if (_actionState == kActionStateRun)
	{
		_velocity = ccp(direction.x * _runSpeed, direction.y * _runSpeed);
		this->flipSpriteForVelocity(_velocity);
		this->jumpRise();
	}
}

void ActionSprite::jumpRise()
{
	if (_actionState == kActionStateIdle || _actionState == kActionStateWalk ||
		_actionState == kActionStateRun || _actionState == kActionStateJumpLand)
	{
		this->stopAllActions();
		this->runAction(_jumpRiseAction);
		_jumpVelocity = kJumpForce;
		//this->_actionState = kActionStateJumpRise;
		this->setActionState(kActionStateJumpRise);
	}
}

void ActionSprite::jumpCutoff()
{
	if (_actionState == kActionStateJumpRise)
	{
		//这允许玩家通过释放跳跃按钮来中断跳跃，使精灵下降更快
		if (_jumpVelocity > kJumpCutoff)
		{
			_jumpVelocity = kJumpCutoff;
		}
	}
}

void ActionSprite::jumpFall()
{
	if (_actionState == kActionStateJumpRise || _actionState == kActionStateJumpAttack)
	{
		//this->_actionState = kActionStateJumpFall;
		this->setActionState(kActionStateJumpFall);
		//this->stopAllActions();
		this->runAction(_jumpFallAction);
	}
}

void ActionSprite::jumpLand()
{
	if (_actionState == kActionStateJumpFall || _actionState == kActionStateRecover)
	{
		_jumpHeight = 0;
		_jumpVelocity = 0;
		_didJumpAttack = false;
		//this->_actionState = kActionStateJumpLand;
		this->setActionState(kActionStateJumpLand);
		this->runAction(_jumpLandAction);
	}
}

void ActionSprite::update(float dt)
{
	CCPoint _position = this->getPosition();
	if (_actionState == kActionStateWalk || _actionState == kActionStateRun ||
		_actionState == kActionStateRunAttack)
	{
		//该循环中会根据用户的触摸计算出预期要移动到的目标点，在GameLayer中会移动hero到该点
		_desiredPosition = ccpAdd(_groundPosition, ccpMult(_velocity, dt));//计算角色期望到达的地方
	}
	else if(_actionState == kActionStateJumpRise) //向上跳
	{
		_desiredPosition = ccpAdd(_groundPosition, ccpMult(_velocity, dt));
		_jumpVelocity -= kGravity * dt;
		_jumpHeight += _jumpVelocity * dt;
		
		if (_jumpVelocity <= kJumpForce/2)
		{
			this->jumpFall();
		}
	}
	else if (_actionState == kActionStateJumpFall)
	{
		_desiredPosition = ccpAdd(_groundPosition, ccpMult(_velocity, dt));
		_jumpVelocity -= kGravity * dt;
		_jumpHeight += _jumpVelocity * dt;

		if(_jumpHeight <= 0)  //jumpVelocity因速度为负会变小直至为0，为0时即为着落
			this->jumpLand();
	}
	else if (_actionState == kActionStateKnockedOut) //处于被击倒状态
	{
		//被击倒后，不应该马上待用恢复或者死亡，特别是还处于空中的时候

		_desiredPosition = ccpAdd(_groundPosition, ccpMult(_velocity, dt));
		//再模拟一遍跳跃过程
		_jumpVelocity -= kGravity * dt;
		_jumpHeight += _jumpVelocity * dt;

		if (_jumpHeight <=0) //倒地了
		{
			if (_hitPoints <= 0) //没血了，就挂
			{
				this->die();
			}
			else
				this->recover(); //有血，就起来
		}
	}
	else if (_actionState == kActionStateAutomated)
	{
		//角色自动行走时位置是直接给出的
		this->setGroundPosition(_position);
		_desiredPosition = _groundPosition;
	}

	//CCLog("_actionDelay:%f", _actionDelay);
	if (_actionDelay > 0)
	{
		_actionDelay -= dt;
		if (_actionDelay <= 0)
		{
			_actionDelay = 0;
		}
	}

	//CCLabelTTF* lbl = GameScene::inst()->_hudLayer->_lblTest;
	//CCString *str = CCString::createWithFormat("actionDelay:%f", _actionDelay);
	//lbl->setString(str->getCString());
}

void ActionSprite::attack()
{
	if (_actionState == kActionStateIdle || _actionState == kActionStateWalk ||
		(_actionState == kActionStateAttack && _actionDelay<=0))
	{
		//CCLog("_actionState: %d", _actionState);
		//CCLog("attack");
		this->stopAllActions();
		this->runAction(_attackAction);
		this->setActionState(kActionStateAttack);
		_actionDelay = _attackDelayTime;
	}
	else if (_actionState == kActionStateJumpRise || _actionState == kActionStateJumpFall)
	{
		this->jumpAttack();
	}
	else if (_actionState == kActionStateRun)
	{
		this->runAttack();
	}
}

AnimationMember* ActionSprite::animationMemberWithPrefix(const char* prefix, int startFrameIdx, int frameCount, float delay, cocos2d::CCSprite* target)
{
	CCAnimation* animation = this->animationWithPrefix(prefix, startFrameIdx, frameCount, delay);
	return AnimationMember::memberWithAnimation(animation, target);
}

ContactPoint ActionSprite::contactPointWithOffset(const cocos2d::CCPoint offset, const float radius)
{
	ContactPoint contactPoint;
	contactPoint.offset = ccpMult(offset, kPointFactor);
	contactPoint.radius = radius * kPointFactor;
	contactPoint.position = ccpAdd(this->getPosition(), contactPoint.offset);
	return contactPoint;
}

void ActionSprite::modifyPoint(ContactPoint *point, const cocos2d::CCPoint offset, const float radius)
{
	point->offset = ccpMult(offset, kPointFactor);
	//point->offset = ccp(offset.x * this->_directionX, offset.y); //注意要乘以朝向
	CCPoint real_offset = ccp(offset.x * this->_directionX, offset.y);//* this->_directionX

	point->radius = radius * kPointFactor;
	point->position = ccpAdd(this->getPosition(), real_offset);
}

void ActionSprite::modifyAttackPointAtIndex(const int pointIndex, const cocos2d::CCPoint offset, const float radius)
{
	ContactPoint *contactPoint = &(_attackPoints[pointIndex]);
	if(contactPoint==NULL)
		return;

	this->modifyPoint(contactPoint, offset, radius);
}

void ActionSprite::modifyContactPointAtIndex(int pointIndex, const cocos2d::CCPoint offset, const float radius)
{
	ContactPoint *contactPoint = &(_contactPoints[pointIndex]);
	this->modifyPoint(contactPoint, offset, radius);
}

void ActionSprite::setActionState(ActionState actionState)
{
	_actionState = actionState;
	this->setContactPointsForAction(actionState);
}

void ActionSprite::setContactPointsForAction(ActionState actionState)
{
	//基于状态重新安排检测圆，由子类实现
}

void ActionSprite::transformPoints()
{
	float pixelScaleX = this->getScaleX() / CC_CONTENT_SCALE_FACTOR();
	float pixelScaleY = this->getScaleY() / CC_CONTENT_SCALE_FACTOR();
	int i;
	//针对每一个判定点来根据玩家的位置重新计算位置
	for (i=0; i<_contactPointCount; i++)
	{
		_contactPoints[i].position = ccpAdd(this->getPosition(), ccp(_contactPoints[i].offset.x * pixelScaleX , //* this->_directionX
			_contactPoints[i].offset.y * pixelScaleY));
	}
	for (i=0; i<_attackPointCount; i++)
	{
		_attackPoints[i].position = ccpAdd(this->getPosition(), ccp(_attackPoints[i].offset.x * pixelScaleX , // * this->_directionX
			_attackPoints[i].offset.y * pixelScaleY));
	}
}

void ActionSprite::hurtWithDamage(float damage, float force, CCPoint direction)
{
	if (_actionState > kActionStateNone && _actionState < kActionStateKnockedOut)
	{
		if (_jumpHeight > 0) //如果角色跳起来
		{
			this->knockoutWithDamage(damage, direction);
		}
		else //角色在地上
		{
			this->stopAllActions();
			this->runAction(_hurtAction);
			//this->_actionState = kActionStateHurt;
			this->setActionState(kActionStateHurt);
			_hitPoints -= damage; //hp减少
			this->_desiredPosition = ccp(this->getPositionX()+direction.x*force, this->getPositionY());

			if(_hitPoints<=0) //血被扣光倒下
				this->knockoutWithDamage(0, direction);
		}
	}
}

void ActionSprite::knockoutWithDamage(float damage, cocos2d::CCPoint direction)
{
	if (_actionState!=kActionStateKnockedOut && _actionState != kActionStateDead && _actionState!=kActionStateRecover
		&& _actionState!=kActionStateNone)
	{
		_hitPoints -= damage;
		this->stopAllActions();
		this->runAction(_knockedOutAction);

		//被击倒的效果的效果不仅仅是向后退，还要伴随着腾空
		//设置后倒的向上加速度
		_jumpVelocity = kJumpForce / 2.0f;//腾空
		_velocity = ccp(direction.x * _runSpeed, direction.y * _runSpeed);//后退
		this->flipSpriteForVelocity(ccp(-_velocity.x, -_velocity.y)); //设置角色朝向，处理起来后的朝向
		//this->_actionState = kActionStateKnockedOut;
		this->setActionState(kActionStateKnockedOut);
	}
}

void ActionSprite::die()
{
	//this->_actionState = kActionStateDead;
	this->setActionState(kActionStateDead);
	_velocity = CCPointZero;
	_jumpHeight = 0;

	_jumpVelocity = 0;
	_hitPoints = 0.0f;
	//通知委派挂了
	_delegate->actionSpriteDidDie(this);
	this->runAction(_dieAction);
}

void ActionSprite::recover()
{
	if (_actionState == kActionStateKnockedOut)
	{
		//this->_actionState = kActionStateNone;
		this->setActionState(kActionStateNone);
		_velocity = CCPointZero;
		_jumpVelocity = 0;
		_jumpHeight = 0;

		//延时0.5m后调用getUp
		CCSequence* seq = CCSequence::create(CCDelayTime::create(0.5f), CCCallFunc::create(this, callfunc_selector(ActionSprite::getUp)), NULL);
		this->runAction(seq);
	}
}

void ActionSprite::getUp()
{
	//this->_actionState = kActionStateRecover;
	this->setActionState(kActionStateRecover);
	this->runAction(_recoverAction);
}

void ActionSprite::reset()
{
	//this->_actionState = kActionStateNone;
	this->setActionState(kActionStateNone);
	this->setPosition(OFFSCREEN);
	this->setDesiredPosition(OFFSCREEN);
	this->setGroundPosition(OFFSCREEN);
	this->setHitPoints(_maxHitPoints);
}

void ActionSprite::enterFrom(cocos2d::CCPoint origin, cocos2d::CCPoint destination)
{
	float diffX = fabsf(destination.x - origin.x);
	float diffY = fabsf(destination.y - origin.y);

	//设定角色朝向
	if (diffX > 0)
	{
		this->_directionX = 1.0f;
	}
	else
		this->_directionX = -1.0f;

	this->setScaleX(_directionX * kScaleFactor);

	float duration = MAX(diffX, diffY) / this->_walkSpeed; //行走的时间

	//_actionState = kActionStateAutomated; //状态为自动
	this->setActionState(kActionStateAutomated);
	this->stopAllActions();
	this->runAction(_walkAction);

	//先走再停
	CCSequence* seq = CCSequence::create(CCMoveTo::create(duration, destination), CCCallFunc::create(this, callfunc_selector(ActionSprite::idle)), NULL);
																				  
	this->runAction(seq);
}

void ActionSprite::jumpAttack()
{
	if (!_didJumpAttack && (_actionState == kActionStateJumpRise || _actionState == kActionStateJumpFall))
	{
		_velocity = CCPointZero;
		this->stopAllActions();
		//this->_actionState = kActionStateJumpAttack;
		this->setActionState(kActionStateJumpAttack);
		_didJumpAttack = true;
		this->runAction(_jumpAttackAction);
	}
}

void ActionSprite::runAttack()
{
	if (_actionState == kActionStateRun)
	{
		this->stopAllActions();
		//this->_actionState = kActionStateRunAttack;
		this->setActionState(kActionStateRunAttack);
		this->runAction(_runAttackAction);
	}
}

void ActionSprite::setInvincibleState(float time)
{
	this->_isInvincible = true;
	
	//设置角色一段时间后变的有敌
	//CCSequence *seq = CCSequence::create(CCDelayTime::create(time), CCCallFunc::create(this, callfunc_selector(ActionSprite::setVincible)),NULL);
	//this->runAction(seq);
	scheduleOnce(schedule_selector(ActionSprite::setVincible), time);
}

void ActionSprite::setVincible(float dt)
{
	this->_isInvincible = false;
}

void ActionSprite::cleanup()
{
	_idleAction = NULL;
	_walkAction = NULL;
	_runAction = NULL;
	_attackAction = NULL;
	_jumpRiseAction = NULL;
	_jumpFallAction = NULL;
	_jumpLandAction = NULL;
	_hurtAction = NULL;
	_knockedOutAction = NULL;
	_dieAction = NULL;
	_recoverAction = NULL;
	_jumpAttackAction = NULL;
	_runAttackAction = NULL;

	CCSprite::cleanup();
}