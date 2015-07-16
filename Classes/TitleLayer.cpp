#include "TitleLayer.h"
#include "GameScene.h"
#include "Defines.h"

#include "SimpleAudioEngine.h"
USING_NS_CC;
using namespace CocosDenshion;

TitleLayer::TitleLayer()
{

}

TitleLayer::~TitleLayer()
{

}

bool TitleLayer::init()
{
	if (!CCLayer::init())
	{
		return false;
	}

	SimpleAudioEngine::sharedEngine()->preloadEffect(EFFECT_HIT0_WAV);
	SimpleAudioEngine::sharedEngine()->preloadEffect(EFFECT_HIT1_WAV);
	SimpleAudioEngine::sharedEngine()->preloadEffect("Sounds/herodeath.caf");
	SimpleAudioEngine::sharedEngine()->preloadEffect("Sounds/enemydeath.caf");
	SimpleAudioEngine::sharedEngine()->preloadEffect("blip.caf");
	SimpleAudioEngine::sharedEngine()->playBackgroundMusic("Sounds/latin_industries.aifc");
	SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(0.5f);

	setupTitle();
	
	this->setTouchEnabled(true);

	return true;
}

void TitleLayer::setupTitle()
{
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	//背景图片
	CCSprite *titleBG = CCSprite::create("Images/bg_title.png");
	titleBG->setPosition(CENTER);
	this->addChild(titleBG);

	//文字图片
	CCSprite *title = CCSprite::create("Images/txt_title.png");
	title->setPosition(ccp(CENTER.x, CENTER.y+66));
	this->addChild(title);

	//提示点击图片
	CCSprite *start = CCSprite::create("Images/txt_touchtostart.png");
	start->setPosition(ccp(CENTER.x, CENTER.y - 37.5));
	this->addChild(start);

	titleBG->getTexture()->setAliasTexParameters();
	title->getTexture()->setAliasTexParameters();
	start->getTexture()->setAliasTexParameters();

	//实现start的闪铄效果
	start->setTag(1); //标志
	CCBlink* blink = CCBlink::create(5.0f, 10);
	CCRepeatForever *repeat = CCRepeatForever::create(blink);
	start->runAction(repeat);
}

void TitleLayer::registerWithTouchDispatcher()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->addStandardDelegate(this, -1);
}

void TitleLayer::ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
	SimpleAudioEngine::sharedEngine()->playEffect("Sounds/hit0.wav");
	GameScene* scene = GameScene::nodeWithLevel(0);
	CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(1.0f, scene, ccWHITE));

	CCSprite* start = (CCSprite*)(this->getChildByTag(1));
	start->stopAllActions();
	start->setVisible(false);
}

void TitleLayer::ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{

}