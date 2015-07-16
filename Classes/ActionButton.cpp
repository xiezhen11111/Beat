#include "ActionButton.h"
USING_NS_CC;

ActionButton::ActionButton()
{
	_delegate = NULL;
}

ActionButton::~ActionButton()
{
	this->unscheduleUpdate();
}

ActionButton* ActionButton::buttonWithPrefix(const char* filePrefix, float radius)
{
	ActionButton* pRet = new ActionButton();
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

bool ActionButton::initWithFilePrefix(const char *filePrefix, float radius)
{
	bool bRet = false;
	CCString *fileName = CCString::createWithFormat("%s_normal.png",filePrefix);

	do 
	{
		CC_BREAK_IF(!CCSprite::initWithSpriteFrameName(fileName->getCString()));

		_radius = radius;
		_prefix = filePrefix;
		_isHeld = false;

		this->scheduleUpdate();

		bRet = true;
	} while (0);

	return bRet;
}

void ActionButton::update(float dt)
{
	if (_isHeld)
	{
		_delegate->actionButtonIsHeld(this);
	}
}

void ActionButton::onEnterTransitionDidFinish()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 1, true);
}

void ActionButton::onExit()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
}

bool ActionButton::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
	CCPoint location = CCDirector::sharedDirector()->convertToGL(pTouch->getLocationInView());

	float distanceSQ  = ccpDistanceSQ(location, this->getPosition());
	if (distanceSQ <= _radius * _radius) //触摸点的范围在button的半径之内
	{
		_isHeld = true;
		//改变按键图片
		CCString* strName = CCString::createWithFormat("%s_selected.png", _prefix.c_str());
		this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(strName->getCString()));
		_delegate->actionButtonWasPressed(this); //触发代理的按键被按下事件

		return true;
	}

	return false;
}

void ActionButton::ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
	_isHeld = false;
	//改变按键图片
	CCString* strName = CCString::createWithFormat("%s_normal.png", _prefix.c_str());
	this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(strName->getCString()));
	//触发代理的按键释放事件
	_delegate->actionButtonWasReleased(this);
}