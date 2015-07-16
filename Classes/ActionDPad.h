#ifndef _ActionDPad_
#define _ActionDPad_

#include "cocos2d.h"

typedef enum _ActionDPadDirection
{
	kActionDPadDirectionCenter = 0,  //���ĵ�
	kActionDPadDirectionUp,			 //����
	kActionDPadDirectionUpRight,	 //����
	kActionDPadDirectionRight,		 //��
	kActionDPadDirectionDownRight,	 //����
	kActionDPadDirectionDown,		 //��
	kActionDPadDirectionDownLeft,	 //����
	kActionDPadDirectionLeft,		 //��
	kActionDPadDirectionUpLeft		 //����
}ActionDPadDirection;

class ActionDPad;

//������
class ActionDPadDelegate
{
public:
	virtual void didChangeDirectionTo(ActionDPad *actionDPad, ActionDPadDirection direction) = 0; //�ı䷽��
	virtual void isHoldingDirection(ActionDPad *actionDPad, ActionDPadDirection direction) = 0;   //�ǲ��ǰ�ס��ĳ������
	virtual void actionDPadTouchEnded(ActionDPad *actionDPad) = 0;							   //��ѹĳ���������
};

class ActionDPad : public cocos2d::CCSprite, public cocos2d::CCTargetedTouchDelegate
{
public:
	ActionDPad();
	~ActionDPad();

	static ActionDPad* dPadWithFile(const char* filePrefix, float radius);
	bool initWithFilePrefix(const char *filePrefix, float radius); //�ɴ�ǰ׺���ļ���������Pad����

	void onEnterTransitionDidFinish();
	void onExit();
	void update(float dt);

	virtual bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
	virtual void ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
	virtual void ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
	
	void updateDirectionForTouchLocation(cocos2d::CCPoint location); //���ݴ�������locationȷ������

	CC_SYNTHESIZE(ActionDPadDelegate*, _delegate, Delegate);
	CC_SYNTHESIZE(bool, _isHeld, IsHeld);  //�Ƿ����ڰ�ѹ�Ű���

	ActionDPadDirection _direction;
protected:
	float _radius;
	ActionDPadDirection _previousDirection;
	
	std::string _prefix;
};

#endif


