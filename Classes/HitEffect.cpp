#include "HitEffect.h"
USING_NS_CC;

HitEffect::HitEffect()
{
	_effectAction = NULL;
}

HitEffect::~HitEffect()
{

}

bool HitEffect::init()
{
	bool bRet=false;
	do 
	{
		CC_BREAK_IF(!CCSprite::initWithSpriteFrameName("hiteffect_00.png"));

		CCArray* frames = CCArray::createWithCapacity(6);
		int i;
		CCSpriteFrame *frame;
		for (i=0; i<6; i++)
		{
			CCString *str = CCString::createWithFormat("hiteffect_%02d.png",i);
			frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(str->getCString());
			frames->addObject(frame);
		}
		CCSequence* seq = CCSequence::create(CCShow::create(), CCAnimate::create(CCAnimation::createWithSpriteFrames(frames, 1.f/12.f)), CCHide::create(), NULL);
		this->setEffectAction(seq);

		bRet = true;
	} while (0);

	return bRet;
}

void HitEffect::showEffectAtPosition(cocos2d::CCPoint pos)
{
	this->stopAllActions();
	this->setPosition(pos);
	this->runAction(_effectAction);
}