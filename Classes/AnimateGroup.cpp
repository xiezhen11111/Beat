#include "AnimateGroup.h"
#include "AnimationMember.h"
USING_NS_CC;

AnimateGroup::AnimateGroup()
{
	_members = NULL;
}

AnimateGroup::~AnimateGroup()
{
	CC_SAFE_RELEASE_NULL(_members);
}

AnimateGroup* AnimateGroup::actionWithAnimation(cocos2d::CCAnimation *animation, cocos2d::CCArray *members)
{
	AnimateGroup* pRet = new AnimateGroup();
	if (pRet && pRet->initWithAnimation(animation, members))
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

bool AnimateGroup::initWithAnimation(cocos2d::CCAnimation *animation, cocos2d::CCArray *members)
{
	bool bRet = false;
	do 
	{
		CC_BREAK_IF(!CCAnimate::initWithAnimation(animation));
		this->_members = members;
		this->_members->retain();
		bRet = true;
	} while (0);

	return bRet;
}

AnimateGroup* AnimateGroup::actionWithAnimation(cocos2d::CCAnimation *animation,int memberCount)
{
	AnimateGroup* pRet = new AnimateGroup();
	if (pRet && pRet->initWithAnimation(animation, memberCount))
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

bool AnimateGroup::initWithAnimation(cocos2d::CCAnimation *animation, int memberCount)
{
	bool bRet = false;

	do 
	{
		CC_BREAK_IF(!CCAnimate::initWithAnimation(animation));

		this->_members = CCArray::createWithCapacity(memberCount);
		this->_members->retain();
		
		bRet = true;
	} while (0);

	return bRet;
}

void AnimateGroup::startWithTarget(CCNode *pTarget)
{
	CCAnimate::startWithTarget(pTarget);

	AnimationMember* aniMember = NULL;
	CCObject *member = NULL;
	CCARRAY_FOREACH(this->_members, member)
	{
		aniMember = (AnimationMember*)member;
		aniMember->start();
	}
}

void AnimateGroup::stop()
{
	CCAnimate::stop();

	AnimationMember *aniMember = NULL;
	CCObject* member = NULL;
	CCARRAY_FOREACH(_members, member)
	{
		aniMember = (AnimationMember *)member;
		aniMember->stop();
	}
}

void AnimateGroup::update(float dt)
{
	CCAnimate::update(dt);

	int frameIndex = MAX(0, m_nNextFrame - 1);

	AnimationMember *aniMember = NULL;
	CCObject* member = NULL;
	CCARRAY_FOREACH(_members, member)
	{
		aniMember = (AnimationMember *)member;
		aniMember->setFrame(frameIndex);
	}
}