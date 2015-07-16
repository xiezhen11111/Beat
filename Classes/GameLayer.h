/************************************************************************/
/* 游戏层
/*GameLayer will contain game elements such as the characters and the stage
/* 游戏层会包含游戏元素如人物和舞台														*/
/************************************************************************/
#ifndef _GameLayer_
#define _GameLayer_

#include "cocos2d.h"
#include "HudLayer.h"
#include "ActionDPad.h"
#include "ActionButton.h"
#include "Hero.h"
#include "Robot.h"
#include "Boss.h"
#include "Gauntlets.h"

class DamageNumber;
class HitEffect;
class MapObject;
class Weapon;

class GameLayer : public cocos2d::CCLayer, public ActionDPadDelegate, public ActionButtonDelegate, public ActionSpriteDelegate
{
public:
	GameLayer();
	~GameLayer();

	static GameLayer* inst(); 

	CREATE_FUNC(GameLayer);
	bool init();

	void onEnterTransitionDidFinish();

	void initTileMap(const char* mapName); //初始化地图
	void initHero();
	void initRobots();
	void initBrains(); //初始化人工智能
	void initEffects();//初始化特效
	void initBossWithType(BossType boss);  //初始化BOSS
	void initWeapons();
	void initMapObjects();  //初始化地图物体

	void initTestRobot(); //测试用

	virtual void update(float dt);
	void updateEvent();
	virtual void onExit();
	void updatePositions(); 

	//代理方法
	virtual void didChangeDirectionTo(ActionDPad *actionDPad, ActionDPadDirection direction);//DPad改变方向时的处理
	virtual void isHoldingDirection(ActionDPad *actionDPad, ActionDPadDirection direction);  //按住DPad不松时的处理
	virtual void actionDPadTouchEnded(ActionDPad *simpleDPad);								 //DPad按住后松开时的处理

	virtual void actionButtonWasPressed(ActionButton* actionButton);  //按压键
	virtual void actionButtonIsHeld(ActionButton* actionButton);
	virtual void actionButtonWasReleased(ActionButton* actionButton);

	bool actionSpriteDidDie(ActionSprite *actionSprite);  //角色死亡处理
	bool actionSpriteDidAttack(ActionSprite *actionSprite); //角色被攻击处理

	//Dpad的direction转化为向量
	cocos2d::CCPoint vectorForDirection(ActionDPadDirection direction);

	//CC_SYNTHESIZE(HudLayer*, _hud, Hud);
	void setHud(HudLayer* hud);

	void setViewpointCenter(cocos2d::CCPoint position);//跟随角色滚动

	void reorderActors(); //重新安排角色的z-order

	int getZFromYPosition(float yPosition); //根据角色的y坐标计算出它的z-order

	void draw(); //重载CCLayer的绘图函数

	void drawShapesForActionSprite(ActionSprite* sprite);  //绘图一个精灵的碰撞框

	//与其他敌人的碰撞检测
	bool collisionBetweenAttacker(ActionSprite* attacker, ActionSprite* target, cocos2d::CCPoint* position);
	//现箱子等地图物体的碰撞检测
	bool collisionBetweenAttacker(ActionSprite* attacker, MapObject* object, cocos2d::CCPoint* position);

	//武器与玩家的碰撞
	bool collisionBetweenPlayer(ActionSprite* player, Weapon* weapon);

	//加载某关关卡和小兵分布
	void loadLevel(int level);

	//产生小兵
	void spawnEnemies(cocos2d::CCArray *enemies, float origin);

	void cycleEvents();

	void triggerEvent(cocos2d::CCNode *pSender, void* data); //一定时间后触发的事件

	//把地图位置转为地图块坐标
	cocos2d::CCPoint tileCoordForPosition(cocos2d::CCPoint position);

	//把地图块坐标转为位置
	cocos2d::CCPoint tilePositionForCoord(cocos2d::CCPoint coord, cocos2d::CCPoint anchorPoint);

	//精灵与地图物体碰撞
	void objectCollisionsForSprite(ActionSprite *sprite);

	//返回武器状态为未开箱的武器
	Weapon* getWeapon();

	//返回游戏主界面
	void gotoMainUI();

	int _totalLevel;	//总的关卡数
	int _currentLevel;  //当前关卡数

	HudLayer* _hud;  //hud层

	static GameLayer* nodeWithLevel(int level); //根据关卡来初始化gamelayer
	bool initWithLevel(int level);				//根据关卡来初始化gamelayer
	void exitLevel();

	void setEventState(EventState eventState);
	DamageNumber* getDamageNumber();
	HitEffect* getHitEffect();
protected:
	int _activeEnemies;     //当前活跃的敌人
	float _viewPointOffset; //视野偏移
	float _eventCenter;     //??

	cocos2d::CCArray *_battleEvents;      //还没发生的事件集合
	cocos2d::CCDictionary *_currentEvent; //当前发生的事件
	cocos2d::CCArray *_weapons;			  //武器列表
	EventState _eventState;				  //游戏当前的状态

	cocos2d::CCArray *_damageNumbers;     //打击敌人时向上的飘字
	cocos2d::CCArray *_hitEffects;        //打击敌人时的打击效果

	cocos2d::CCArray *_mapObjects;		  //地图上的物体

	Boss *_boss;
private:
	cocos2d::CCTMXTiledMap* _tileMap;      //游戏地图
	cocos2d::CCSpriteBatchNode* _actors;   //角色批处理器
	Hero *_hero;
	float _runDelay;   //跑的时间间隔
	ActionDPadDirection _previousDirection; //上次D-pad返回的方向
	cocos2d::CCArray* _robots;
	cocos2d::CCArray* _brains;
};
#endif