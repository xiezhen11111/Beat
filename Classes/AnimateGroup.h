#ifndef _AnimateGroup_
#define _AnimateGroup_

#include "cocos2d.h"

/**
*
*控制一组AnimationMembers
*机器人的动画是有三部分组成：base，烟，腰带
*如果分开播放各自的动画，有很大的可能性他们会不同步,尤其是你正在处理很短的帧间隔
*所以这里使用了动画组AnimateGroup，这样当播放base动画的时候，在更新动画帧的时候同时也会更新另外两个动画，从而保证了同步播放三组动画
*
**/
class AnimateGroup : public cocos2d::CCAnimate
{
public:
	AnimateGroup();
	~AnimateGroup();
	//用数组来初始化函数 
	static AnimateGroup* actionWithAnimation(cocos2d::CCAnimation *animation, cocos2d::CCArray *members);
	bool initWithAnimation(cocos2d::CCAnimation *animation, cocos2d::CCArray *members); //用动画和数组来初始化
	//用成员数来初始化
	static AnimateGroup* actionWithAnimation(cocos2d::CCAnimation *animation, int memberCount);
	bool initWithAnimation(cocos2d::CCAnimation *animation, int memberCount); //用动画和数组数来初始化

	void startWithTarget(cocos2d::CCNode *pTarget);
	void stop();  //所有动画停止

	void update(float dt);

	cocos2d::CCArray* _members;  //动画成员
protected:
	
private:
};
#endif