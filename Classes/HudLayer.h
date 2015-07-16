#ifndef _HudLayer_
#define _HudLayer_

#include "cocos2d.h"
#include "ActionDPad.h"
#include "ActionButton.h"


/************************************************************************/
/* Hud层
/*while HudLayer will contain display elements 
such as the directional pad and the A and B buttons. HudLayer needs to be drawn 
last because it has to be on top of everything and always visible
/*Hud层会包含显示元素比如方向键和AB按钮，Hud层需要在最后绘制因为它要显示在所有东西的前面并且一直显示 														
/************************************************************************/
class HudLayer : public cocos2d::CCLayer
{
public:
	HudLayer();
	~HudLayer();

	CREATE_FUNC(HudLayer);
	bool init();

	CC_SYNTHESIZE_RETAIN(ActionDPad*, _dPad, DPad);  //方向键
	ActionButton* _buttonA;
	ActionButton* _buttonB;

	cocos2d::CCLabelBMFont *_hitPointsLabel;  //显示血量用
	cocos2d::CCLabelBMFont *_goLabel;		  //显示GoGo
	cocos2d::CCLabelBMFont *_centerLabel;	  //显示于屏幕中心处的指示信息
	cocos2d::CCLabelTTF* _lblTest;  //测试用的label

	void setHitPoints(float newHP, float maxHP); //显示主角血量
	void showGoMessage();
	void displayLevel(int level);		//显示当前关卡数
	void showMessage(const char* message, cocos2d::ccColor3B color); //显示信息用

protected:
	//ActionDPad *_dPad; //键位按钮
private:
};
#endif