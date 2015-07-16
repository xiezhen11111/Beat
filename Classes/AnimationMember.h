#ifndef _AnimationMember_
#define _AnimationMember_

#include "cocos2d.h"

class AnimationMember : public cocos2d::CCObject
{
public:
	AnimationMember();
	~AnimationMember();

	static AnimationMember* memberWithAnimation(cocos2d::CCAnimation *animation, cocos2d::CCSprite *target);
	bool initWithAnimation(cocos2d::CCAnimation *animation, cocos2d::CCSprite *target); //用动画和播放对象来初始化

	void start();   //开始播放动画
	void stop();	//停止播放动画
	void setFrame(int frameIndex); //设置播放动画的对象(_target)图片为动画中的某一帧
protected:
	cocos2d::CCSpriteFrame* _origFrame;  //初始帧
	cocos2d::CCAnimation* _animation;	 //动画
	cocos2d::CCSprite *_target;			 //谁在播放动画
private:
};
#endif