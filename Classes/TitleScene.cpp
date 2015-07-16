#include "TitleScene.h"
#include "TitleLayer.h"

TitleScene::TitleScene()
{

}

TitleScene::~TitleScene()
{

}

bool TitleScene::init()
{
	if (!CCScene::init())
	{
		return false;
	}
	
	TitleLayer* titleLayer = TitleLayer::create();
	this->addChild(titleLayer);

	return true;
}