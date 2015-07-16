#include "TrashCan.h"
USING_NS_CC;

TrashCan::TrashCan()
{

}

TrashCan::~TrashCan()
{

}

bool TrashCan::init()
{
	bool bRet = true;

	do 
	{
		CC_BREAK_IF(!CCSprite::initWithSpriteFrameName("trashcan.png"));
		this->_objectState = kObjectStateActive;
		this->_detectionRadius = 57.f * kPointFactor;
		this->_contactPointCount = 5;
		int i = 0;
		for (i=0; i<_contactPointCount; i++)
		{
			ContactPoint cp;
			_contactPoints.push_back(cp);
		}
		//设置下箱子的各个接触点
		this->modifyContactPointAtIndex(0, ccp(0.0f, 2.f), 33.f);
		this->modifyContactPointAtIndex(1, ccp(0.f, -15.f), 33.f);
		this->modifyContactPointAtIndex(2, ccp(0.f, 26.f), 17.f);
		this->modifyContactPointAtIndex(3, ccp(19.f, 29.f), 16.f);
		this->modifyContactPointAtIndex(4, ccp(-23.f, -38.f), 10.f);
	} while (0);
	return bRet;
}

void TrashCan::destroyed()
{
	CCSpriteFrame *destroyedFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("trashcan_hit.png");
	this->setDisplayFrame(destroyedFrame);
	MapObject::destroyed();
}

CCRect TrashCan::collisionRect()
{
	CCPoint pos = this->getPosition();
	CCSize size = this->getContentSize();
	//以箱子pos为中心，大小为64*32范围生成一个碰撞检测框
	return CCRectMake(pos.x - size.width/2 * kScaleFactor, pos.y-size.height/2 * kScaleFactor, 64*kPointFactor, 32*kPointFactor);
}