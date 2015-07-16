#include "ActionDPad.h"
USING_NS_CC;

ActionDPad::ActionDPad()
{
	_delegate = NULL;
}

ActionDPad::~ActionDPad()
{
	this->unscheduleUpdate();
}

ActionDPad* ActionDPad::dPadWithFile(const char* filePrefix, float radius)
{
	ActionDPad* pRet = new ActionDPad();
	if (pRet && pRet->initWithFilePrefix(filePrefix, radius))
	{
		return pRet;
	} 
	else
	{
		delete pRet;
		pRet = NULL;
		return pRet;
	}
}

bool ActionDPad::initWithFilePrefix(const char *filePrefix, float radius)
{
	bool bRet = false;
	CCString *filename = CCString::createWithFormat("%s_center.png", filePrefix);
	do 
	{
		CC_BREAK_IF(!CCSprite::initWithSpriteFrameName(filename->getCString()));

		_radius = radius;
		_direction = kActionDPadDirectionCenter;
		_previousDirection = kActionDPadDirectionCenter;
		_isHeld = false;
		_prefix = filePrefix;
		this->scheduleUpdate();

		bRet = true;
	} while (0);

	return bRet;
}

void ActionDPad::onEnterTransitionDidFinish()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 1, true); //添加本层的触摸响应，并吞掉传向下层的响应，这样GameLayer层就不会接收到响应了
}

void ActionDPad::onExit()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
}

void ActionDPad::update(float dt)
{
	if (_isHeld)
	{
		_delegate->isHoldingDirection(this, _direction);
	}
}

bool ActionDPad::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
	CCPoint location = pTouch->getLocation();

	float distanceSQ = ccpDistanceSQ(location, this->getPosition());
	//是否点击在摇杆中
	if (distanceSQ <= _radius * _radius)
	{
		this->updateDirectionForTouchLocation(location);
		_isHeld = true;
		return true;
	}
	return false;
}

void ActionDPad::ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
	//随着移动更新按下的方向
	CCPoint location = pTouch->getLocation();
	this->updateDirectionForTouchLocation(location);
}

void ActionDPad::ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
	_direction = kActionDPadDirectionCenter;
	_isHeld = false;
	//恢复方向键显示图片
	CCString* strName = CCString::createWithFormat("%s_center.png",_prefix.c_str());
	this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(strName->getCString()));
	_delegate->actionDPadTouchEnded(this);//通知触摸结束
}

//location即是玩家的触摸点，计算出所处方向
void ActionDPad::updateDirectionForTouchLocation(cocos2d::CCPoint location)
{
	//1.根据触摸点与中心点的位置计算出角度
	float radians = ccpToAngle(ccpSub(location, this->getPosition())); //根据玩家触摸点和方向PAD的中心点计算角度
	float degrees = -1 * CC_RADIANS_TO_DEGREES(radians); //弧度转换为角度，由于cocos2dx中坐标系（原点在左下角）与openGL坐标系（原点在左上角）是相反的，所以需要乘以-1
	CCString *suffix = CCString::create("_center.png");

	_previousDirection = _direction;

	//2.根据获得的角度判断所处的方向
	if (degrees <= 22.5 && degrees >= -22.5)
	{
		_direction = kActionDPadDirectionRight; 
		suffix = CCString::create("_right.png");
	}
	else if (degrees > 22.5 && degrees <67.5)
	{
		_direction = kActionDPadDirectionDownRight;
		suffix = CCString::create("_downright.png");
	}
	else if (degrees >= 67.5 && degrees <= 112.5)
	{
		_direction = kActionDPadDirectionDown;
		suffix = CCString::create("_down.png");
	}
	else if (degrees > 112.5 && degrees < 157.5)
	{
		_direction = kActionDPadDirectionDownLeft;
		suffix = CCString::create("_downleft.png");
	}
	else if (degrees >= 157.5 || degrees <= -157.5)
	{
		_direction = kActionDPadDirectionLeft;
		suffix = CCString::create("_left.png");
	}
	else if (degrees < -22.5 && degrees > -67.5)
	{
		_direction = kActionDPadDirectionUpRight;
		suffix = CCString::create("_upright.png");
	}
	else if (degrees <= -67.5 && degrees >= -112.5)
	{
		_direction = kActionDPadDirectionUp;
		suffix = CCString::create("_up.png");
	}
	else if (degrees < -112.5 && degrees > -157.5)
	{
		_direction = kActionDPadDirectionUpLeft;
		suffix = CCString::create("_upleft.png");
	}

	//3.只要有新的方向产生就会传递给委派类
	CCString* filename = CCString::create("");
	filename = CCString::createWithFormat("%s%s",_prefix.c_str(), suffix->getCString());
	this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(filename->getCString()));

	if (_isHeld) //按压时要考虑之前按压状态，已经在按了就不要重复设置方向了，未按压直接设置方向
	{
		if (_previousDirection != _direction)
		{
			_delegate->didChangeDirectionTo(this, _direction); //设置方向_,
		}
	}
	else
	{
		_delegate->didChangeDirectionTo(this, _direction);
	}
}
