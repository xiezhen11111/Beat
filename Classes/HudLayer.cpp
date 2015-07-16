#include "HudLayer.h"
#include "Defines.h"

USING_NS_CC;

HudLayer::HudLayer()
{
	_dPad = NULL;
	_buttonA = NULL;
	_buttonB = NULL;
	_lblTest = NULL;
	_centerLabel = NULL;
}

HudLayer::~HudLayer()
{

}

bool HudLayer::init()
{
	bool bRet = false;

	do 
	{
		CC_BREAK_IF(!CCLayer::init());

		//��ʾ�����
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Images/joypad.plist");

		float radius = 64.0 * kPointFactor;
		_dPad = ActionDPad::dPadWithFile("dpad", radius);
		_dPad->setPosition(ccp(radius, radius));
		_dPad->setOpacity(128);
		this->addChild(_dPad);

		//��ʾ���������İ�ť(A�ǹ�����B����)
		float buttonRadius = radius / 2.0;
		float padding = 8.0 * kPointFactor;

		_buttonB = ActionButton::buttonWithPrefix("button_b", buttonRadius);
		_buttonB->setPosition(ccp(SCREEN.width - buttonRadius - padding,
			buttonRadius * 2 + padding));
		_buttonB->setOpacity(128);
		_buttonB->setTag(kTagButtonB);
		this->addChild(_buttonB);

		_buttonA = ActionButton::buttonWithPrefix("button_a", buttonRadius);
		_buttonA->setPosition(ccp(_buttonB->getPosition().x-radius-padding,
			buttonRadius+padding));
		_buttonA->setOpacity(128);
		_buttonA->setTag(kTagButtonA);

		this->addChild(_buttonA);

		//��ʾ����Ѫ���ı�ǩ
		float xPadding = 10.0f * kPointFactor;
		float yPadding = 18.0f * kPointFactor;
		_hitPointsLabel = CCLabelBMFont::create("0", "Fonts/HudFont.fnt");
		_hitPointsLabel->setAnchorPoint(ccp(0.f, 0.5f));
		_hitPointsLabel->setPosition(ccp(xPadding, SCREEN.height - 
			_hitPointsLabel->getContentSize().height/2 - yPadding));
		this->addChild(_hitPointsLabel);

		//��ʾgogo��ǩ
		_goLabel = CCLabelBMFont::create("GO>","Fonts/HudFont.fnt");
		_goLabel->setAnchorPoint(ccp(1.0f, 0.5f));
		_goLabel->setPosition(ccp(SCREEN.width-xPadding, SCREEN.height-_goLabel->getContentSize().height/2-yPadding));
		_goLabel->setColor(COLOR_FULLHP);
		_goLabel->setVisible(false);
		this->addChild(_goLabel);

		//��ʾ�м�ؿ�����Ϣ
		_centerLabel = CCLabelBMFont::create("Level 1", "Fonts/HudFont.fnt");
		_centerLabel->setColor(COLOR_MIDHP);
		_centerLabel->setVisible(false);
		this->addChild(_centerLabel);

		//�����õ�test��ǩ�����ڼ����Ϸ���ݣ�ʵ�ʹ����в�Ҫ��ʾ��
		_lblTest = CCLabelTTF::create();
		_lblTest->setFontFillColor(ccWHITE);
		_lblTest->setFontSize(20);
		_lblTest->setAnchorPoint(ccp(0,0));
		_lblTest->setPosition(ccp(20, 270));
		this->addChild(_lblTest);

		bRet = true;
	} while (0);

	return bRet;
}

void HudLayer::setHitPoints(float newHP, float maxHP)
{
	int wholeHP = newHP;

	_hitPointsLabel->setString(CCString::createWithFormat("%d", wholeHP)->getCString());

	float ratio = newHP / maxHP;
	if (ratio>0.6)
	{
		_hitPointsLabel->setColor(COLOR_FULLHP);
	}
	else if (ratio>0.2)
	{
		_hitPointsLabel->setColor(COLOR_MIDHP);
	}
	else
		_hitPointsLabel->setColor(COLOR_LOWHP);
}

void HudLayer::showGoMessage()
{
	_goLabel->stopAllActions();
	//CCShow��ʵ���Ǽ򵥵İ�visible��Ϊtrue����������3����6�Σ��ٺ�����
	CCSequence* seq = CCSequence::create(CCShow::create(), CCBlink::create(3.f, 6), CCHide::create(), NULL);
	_goLabel->runAction(seq);
}

void HudLayer::displayLevel(int level)
{
	_centerLabel->stopAllActions();
	_centerLabel->setString(CCString::createWithFormat("LEVEL %d", level)->getCString());
	//_centerLabel->setString("LEVEL 1");
	CCPoint pos = ccp(SCREEN.width+_centerLabel->getContentSize().width/2, CENTER.y);
	//�Ȱѱ�־�ŵ���Ļ���ұߣ��ٳ��֣�2�����Ƶ���Ļ���ģ��ٹ�һ�����Ƴ���Ļ�������ʧ
	CCSequence *seq = CCSequence::create(CCPlace::create(pos),CCShow::create(), CCMoveTo::create(0.2f, CENTER),
		CCDelayTime::create(1.f), CCMoveTo::create(0.2f,ccp(-_centerLabel->getContentSize().width/2, CENTER.y)),CCHide::create(),NULL);
	_centerLabel->runAction(seq);
}

void HudLayer::showMessage(const char* message, cocos2d::ccColor3B color)
{
	_centerLabel->setColor(color);
	_centerLabel->setString(message);
	CCSequence* actions = CCSequence::create(CCPlace::create(ccp(SCREEN.width+_centerLabel->getContentSize().width/2, CENTER.y)),
		CCShow::create(), CCMoveTo::create(0.2f, CENTER), CCDelayTime::create(1.f), 
		CCMoveTo::create(0.2f, ccp(-_centerLabel->getContentSize().width/2, CENTER.y)), CCHide::create(),NULL);
	_centerLabel->runAction(actions);
}