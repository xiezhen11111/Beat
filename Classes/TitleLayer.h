#ifndef _TitleLayer_
#define _TitleLayer_

#include "cocos2d.h"

class TitleLayer : public cocos2d::CCLayer
{
public:
	TitleLayer();
	~TitleLayer();

	CREATE_FUNC(TitleLayer);
	bool init();

	//ע�ᴥ���¼�
	virtual void registerWithTouchDispatcher();
	void ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
	void ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);

	void setupTitle(); //��������
protected:
private:
};
#endif