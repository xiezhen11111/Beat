#ifndef _ActionButton_
#define _ActionButton_

#include "cocos2d.h"

class ActionButton;

//代理类
class ActionButtonDelegate
{
public:
	virtual void actionButtonWasPressed(ActionButton* actionButton) = 0;
	virtual void actionButtonIsHeld(ActionButton* actionButton) = 0;
	virtual void actionButtonWasReleased(ActionButton* actionButton) = 0;
};

class ActionButton : public cocos2d::CCSprite, public cocos2d::CCTargetedTouchDelegate
{
public:
	ActionButton();
	~ActionButton();

	static ActionButton* buttonWithPrefix(const char* filePrefix, float radius);
	bool initWithFilePrefix(const char *filePrefix, float radius); //由带前缀的文件名来生成Pad精灵

	void onEnterTransitionDidFinish();
	void onExit();

	virtual bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
	//virtual void ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
	virtual void ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);

	virtual void update(float dt);

	CC_SYNTHESIZE(ActionButtonDelegate*, _delegate, Delegate);
	CC_SYNTHESIZE(bool, _isHeld, IsHeld);  //是否正在按压着按键
protected:
private:
	float _radius;
	std::string _prefix;
};
#endif