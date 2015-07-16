#include "AnimationMember.h"
USING_NS_CC;

AnimationMember::AnimationMember()
{
	_target = NULL;
	_origFrame = NULL;
	_animation = NULL;
}

AnimationMember::~AnimationMember()
{
	CC_SAFE_RELEASE_NULL(_animation);
	CC_SAFE_RELEASE_NULL(_target);
}

AnimationMember* AnimationMember::memberWithAnimation(cocos2d::CCAnimation *animation, cocos2d::CCSprite *target)
{
	AnimationMember* pRet = new AnimationMember();
	if (pRet && pRet->initWithAnimation(animation, target))
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

bool AnimationMember::initWithAnimation(cocos2d::CCAnimation *animation, cocos2d::CCSprite *target)
{
	bool bRet = false;
	do 
	{
		//CC_BREAK_IF(!)
		this->_animation = animation;
		this->_target = target;
		this->_animation->retain();
		this->_target->retain();
		_origFrame = NULL;
		bRet = true;
	} while (0);

	return bRet;
}

void AnimationMember::start()
{
	_origFrame = _target->displayFrame(); //取得当前显示的帧作为初始帧
}

void AnimationMember::stop()
{
	bool bRestore = _animation->getRestoreOriginalFrame(); //播放完成后是否恢复第一帧
	if (bRestore)
	{
		_target->setDisplayFrame(_origFrame); //恢复第一帧
	}
}

void AnimationMember::setFrame(int frameIndex)
{
	CCArray* frames = _animation->getFrames();
	int nCount = frames->count();
	if (frameIndex>=nCount)
	{
		CCLog("AnimationMember setFrame frameindex is greater than framecount");
		return;
	}
	//从动画里取得index帧
	CCAnimationFrame *frame = (CCAnimationFrame *)(frames->objectAtIndex(frameIndex));
	CCSpriteFrame *spriteFrame = frame->getSpriteFrame();
	_target->setDisplayFrame(spriteFrame);
}