#ifndef _HudLayer_
#define _HudLayer_

#include "cocos2d.h"
#include "ActionDPad.h"
#include "ActionButton.h"


/************************************************************************/
/* Hud��
/*while HudLayer will contain display elements 
such as the directional pad and the A and B buttons. HudLayer needs to be drawn 
last because it has to be on top of everything and always visible
/*Hud��������ʾԪ�ر��緽�����AB��ť��Hud����Ҫ����������Ϊ��Ҫ��ʾ�����ж�����ǰ�沢��һֱ��ʾ 														
/************************************************************************/
class HudLayer : public cocos2d::CCLayer
{
public:
	HudLayer();
	~HudLayer();

	CREATE_FUNC(HudLayer);
	bool init();

	CC_SYNTHESIZE_RETAIN(ActionDPad*, _dPad, DPad);  //�����
	ActionButton* _buttonA;
	ActionButton* _buttonB;

	cocos2d::CCLabelBMFont *_hitPointsLabel;  //��ʾѪ����
	cocos2d::CCLabelBMFont *_goLabel;		  //��ʾGoGo
	cocos2d::CCLabelBMFont *_centerLabel;	  //��ʾ����Ļ���Ĵ���ָʾ��Ϣ
	cocos2d::CCLabelTTF* _lblTest;  //�����õ�label

	void setHitPoints(float newHP, float maxHP); //��ʾ����Ѫ��
	void showGoMessage();
	void displayLevel(int level);		//��ʾ��ǰ�ؿ���
	void showMessage(const char* message, cocos2d::ccColor3B color); //��ʾ��Ϣ��

protected:
	//ActionDPad *_dPad; //��λ��ť
private:
};
#endif