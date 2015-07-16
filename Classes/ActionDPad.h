#ifndef _ActionDPad_
#define _ActionDPad_

#include "cocos2d.h"

typedef enum _ActionDPadDirection
{
	kActionDPadDirectionCenter = 0,  //中心点
	kActionDPadDirectionUp,			 //向上
	kActionDPadDirectionUpRight,	 //上右
	kActionDPadDirectionRight,		 //右
	kActionDPadDirectionDownRight,	 //下右
	kActionDPadDirectionDown,		 //下
	kActionDPadDirectionDownLeft,	 //下左
	kActionDPadDirectionLeft,		 //左
	kActionDPadDirectionUpLeft		 //上左
}ActionDPadDirection;

class ActionDPad;

//代理类
class ActionDPadDelegate
{
public:
	virtual void didChangeDirectionTo(ActionDPad *actionDPad, ActionDPadDirection direction) = 0; //改变方向
	virtual void isHoldingDirection(ActionDPad *actionDPad, ActionDPadDirection direction) = 0;   //是不是按住了某个方向
	virtual void actionDPadTouchEnded(ActionDPad *actionDPad) = 0;							   //按压某个方向结束
};

class ActionDPad : public cocos2d::CCSprite, public cocos2d::CCTargetedTouchDelegate
{
public:
	ActionDPad();
	~ActionDPad();

	static ActionDPad* dPadWithFile(const char* filePrefix, float radius);
	bool initWithFilePrefix(const char *filePrefix, float radius); //由带前缀的文件名来生成Pad精灵

	void onEnterTransitionDidFinish();
	void onExit();
	void update(float dt);

	virtual bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
	virtual void ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
	virtual void ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
	
	void updateDirectionForTouchLocation(cocos2d::CCPoint location); //根据传过来的location确定方向

	CC_SYNTHESIZE(ActionDPadDelegate*, _delegate, Delegate);
	CC_SYNTHESIZE(bool, _isHeld, IsHeld);  //是否正在按压着按键

	ActionDPadDirection _direction;
protected:
	float _radius;
	ActionDPadDirection _previousDirection;
	
	std::string _prefix;
};

#endif


