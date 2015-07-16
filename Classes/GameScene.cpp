#include "GameScene.h"

USING_NS_CC;

static GameScene *s_GameScene = NULL;

GameScene::GameScene()
{

}

GameScene::~GameScene()
{

}

GameScene* GameScene::inst()
{
	if (!s_GameScene)
	{
		s_GameScene = GameScene::create();
	}

	return s_GameScene;
}

bool GameScene::init()
{
	bool bRet = false;
	do 
	{
		CC_BREAK_IF(!CCScene::init());

		_gameLayer = GameLayer::create();
		this->addChild(_gameLayer, 0);
		_hudLayer = HudLayer::create();
		this->addChild(_hudLayer, 1);

		_hudLayer->getDPad()->setDelegate(_gameLayer);
		_hudLayer->_buttonA->setDelegate(_gameLayer);
		_hudLayer->_buttonB->setDelegate(_gameLayer);
		_gameLayer->setHud(_hudLayer);

		bRet = true;
	} while (0);

	return bRet;
}

GameScene* GameScene::nodeWithLevel(int level)
{
	GameScene* pRet = new GameScene();
	if (pRet && pRet->initWithLevel(level))
	{
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return pRet;
	}
}

bool GameScene::initWithLevel(int level)
{
	bool bRet = false;

	do 
	{
		CC_BREAK_IF(!CCScene::init());

		_gameLayer = GameLayer::nodeWithLevel(level);
		this->addChild(_gameLayer, 0);
		_hudLayer = HudLayer::create();
		this->addChild(_hudLayer, 1);

		_hudLayer->getDPad()->setDelegate(_gameLayer);
		_gameLayer->setHud(_hudLayer);

		_hudLayer->_buttonA->setDelegate(_gameLayer);
		_hudLayer->_buttonB->setDelegate(_gameLayer);
		

		bRet = true;
	} while (0);

	return bRet;
}