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

		//显示方向键
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Images/joypad.plist");

		float radius = 64.0 * kPointFactor;
		_dPad = ActionDPad::dPadWithFile("dpad", radius);
		_dPad->setPosition(ccp(radius, radius));
		_dPad->setOpacity(128);
		this->addChild(_dPad);

		//显示攻击和跳的按钮(A是攻击，B是跳)
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

		//显示主角血量的标签
		float xPadding = 10.0f * kPointFactor;
		float yPadding = 18.0f * kPointFactor;
		_hitPointsLabel = CCLabelBMFont::create("0", "Fonts/HudFont.fnt");
		_hitPointsLabel->setAnchorPoint(ccp(0.f, 0.5f));
		_hitPointsLabel->setPosition(ccp(xPadding, SCREEN.height - 
			_hitPointsLabel->getContentSize().height/2 - yPadding));
		this->addChild(_hitPointsLabel);

		//显示gogo标签
		_goLabel = CCLabelBMFont::create("GO>","Fonts/HudFont.fnt");
		_goLabel->setAnchorPoint(ccp(1.0f, 0.5f));
		_goLabel->setPosition(ccp(SCREEN.width-xPadding, SCREEN.height-_goLabel->getContentSize().height/2-yPadding));
		_goLabel->setColor(COLOR_FULLHP);
		_goLabel->setVisible(false);
		this->addChild(_goLabel);

		//显示中间关卡数信息
		_centerLabel = CCLabelBMFont::create("Level 1", "Fonts/HudFont.fnt");
		_centerLabel->setColor(COLOR_MIDHP);
		_centerLabel->setVisible(false);
		this->addChild(_centerLabel);

		//测试用的test标签，用于监控游戏数据，实际工程中不要显示它
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
	//CCShow其实就是简单的把visible设为true，其他的再3秒闪6次，再后隐藏
	CCSequence* seq = CCSequence::create(CCShow::create(), CCBlink::create(3.f, 6), CCHide::create(), NULL);
	_goLabel->runAction(seq);
}

void HudLayer::displayLevel(int level)
{
	_centerLabel->stopAllActions();
	_centerLabel->setString(CCString::createWithFormat("LEVEL %d", level)->getCString());
	//_centerLabel->setString("LEVEL 1");
	CCPoint pos = ccp(SCREEN.width+_centerLabel->getContentSize().width/2, CENTER.y);
	//先把标志放到屏幕最右边，再出现，2秒内移到屏幕中心，再过一秒左移出屏幕，最后消失
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