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
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 1, true); //��ӱ���Ĵ�����Ӧ�����̵������²����Ӧ������GameLayer��Ͳ�����յ���Ӧ��
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
	//�Ƿ�����ҡ����
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
	//�����ƶ����°��µķ���
	CCPoint location = pTouch->getLocation();
	this->updateDirectionForTouchLocation(location);
}

void ActionDPad::ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
	_direction = kActionDPadDirectionCenter;
	_isHeld = false;
	//�ָ��������ʾͼƬ
	CCString* strName = CCString::createWithFormat("%s_center.png",_prefix.c_str());
	this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(strName->getCString()));
	_delegate->actionDPadTouchEnded(this);//֪ͨ��������
}

//location������ҵĴ����㣬�������������
void ActionDPad::updateDirectionForTouchLocation(cocos2d::CCPoint location)
{
	//1.���ݴ����������ĵ��λ�ü�����Ƕ�
	float radians = ccpToAngle(ccpSub(location, this->getPosition())); //������Ҵ�����ͷ���PAD�����ĵ����Ƕ�
	float degrees = -1 * CC_RADIANS_TO_DEGREES(radians); //����ת��Ϊ�Ƕȣ�����cocos2dx������ϵ��ԭ�������½ǣ���openGL����ϵ��ԭ�������Ͻǣ����෴�ģ�������Ҫ����-1
	CCString *suffix = CCString::create("_center.png");

	_previousDirection = _direction;

	//2.���ݻ�õĽǶ��ж������ķ���
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

	//3.ֻҪ���µķ�������ͻᴫ�ݸ�ί����
	CCString* filename = CCString::create("");
	filename = CCString::createWithFormat("%s%s",_prefix.c_str(), suffix->getCString());
	this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(filename->getCString()));

	if (_isHeld) //��ѹʱҪ����֮ǰ��ѹ״̬���Ѿ��ڰ��˾Ͳ�Ҫ�ظ����÷����ˣ�δ��ѹֱ�����÷���
	{
		if (_previousDirection != _direction)
		{
			_delegate->didChangeDirectionTo(this, _direction); //���÷���_,
		}
	}
	else
	{
		_delegate->didChangeDirectionTo(this, _direction);
	}
}
