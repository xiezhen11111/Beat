#ifndef _GameScene_
#define _GameScene_

#include "cocos2d.h"
#include "GameLayer.h"
#include "HudLayer.h"

class GameScene : public cocos2d::CCScene
{
public:
	GameScene();
	~GameScene();

	static GameScene* inst(void); //����ģʽ,���û򴴽��������ô˷���

	static GameScene* nodeWithLevel(int level);
	bool initWithLevel(int level);

	GameLayer* _gameLayer;
	HudLayer*  _hudLayer;

protected:
	CREATE_FUNC(GameScene);
	virtual bool init();
private:
};
#endif