/************************************************************************/
/* ��Ϸ��
/*GameLayer will contain game elements such as the characters and the stage
/* ��Ϸ��������ϷԪ�����������̨														*/
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

	void initTileMap(const char* mapName); //��ʼ����ͼ
	void initHero();
	void initRobots();
	void initBrains(); //��ʼ���˹�����
	void initEffects();//��ʼ����Ч
	void initBossWithType(BossType boss);  //��ʼ��BOSS
	void initWeapons();
	void initMapObjects();  //��ʼ����ͼ����

	void initTestRobot(); //������

	virtual void update(float dt);
	void updateEvent();
	virtual void onExit();
	void updatePositions(); 

	//������
	virtual void didChangeDirectionTo(ActionDPad *actionDPad, ActionDPadDirection direction);//DPad�ı䷽��ʱ�Ĵ���
	virtual void isHoldingDirection(ActionDPad *actionDPad, ActionDPadDirection direction);  //��סDPad����ʱ�Ĵ���
	virtual void actionDPadTouchEnded(ActionDPad *simpleDPad);								 //DPad��ס���ɿ�ʱ�Ĵ���

	virtual void actionButtonWasPressed(ActionButton* actionButton);  //��ѹ��
	virtual void actionButtonIsHeld(ActionButton* actionButton);
	virtual void actionButtonWasReleased(ActionButton* actionButton);

	bool actionSpriteDidDie(ActionSprite *actionSprite);  //��ɫ��������
	bool actionSpriteDidAttack(ActionSprite *actionSprite); //��ɫ����������

	//Dpad��directionת��Ϊ����
	cocos2d::CCPoint vectorForDirection(ActionDPadDirection direction);

	//CC_SYNTHESIZE(HudLayer*, _hud, Hud);
	void setHud(HudLayer* hud);

	void setViewpointCenter(cocos2d::CCPoint position);//�����ɫ����

	void reorderActors(); //���°��Ž�ɫ��z-order

	int getZFromYPosition(float yPosition); //���ݽ�ɫ��y������������z-order

	void draw(); //����CCLayer�Ļ�ͼ����

	void drawShapesForActionSprite(ActionSprite* sprite);  //��ͼһ���������ײ��

	//���������˵���ײ���
	bool collisionBetweenAttacker(ActionSprite* attacker, ActionSprite* target, cocos2d::CCPoint* position);
	//�����ӵȵ�ͼ�������ײ���
	bool collisionBetweenAttacker(ActionSprite* attacker, MapObject* object, cocos2d::CCPoint* position);

	//��������ҵ���ײ
	bool collisionBetweenPlayer(ActionSprite* player, Weapon* weapon);

	//����ĳ�عؿ���С���ֲ�
	void loadLevel(int level);

	//����С��
	void spawnEnemies(cocos2d::CCArray *enemies, float origin);

	void cycleEvents();

	void triggerEvent(cocos2d::CCNode *pSender, void* data); //һ��ʱ��󴥷����¼�

	//�ѵ�ͼλ��תΪ��ͼ������
	cocos2d::CCPoint tileCoordForPosition(cocos2d::CCPoint position);

	//�ѵ�ͼ������תΪλ��
	cocos2d::CCPoint tilePositionForCoord(cocos2d::CCPoint coord, cocos2d::CCPoint anchorPoint);

	//�������ͼ������ײ
	void objectCollisionsForSprite(ActionSprite *sprite);

	//��������״̬Ϊδ���������
	Weapon* getWeapon();

	//������Ϸ������
	void gotoMainUI();

	int _totalLevel;	//�ܵĹؿ���
	int _currentLevel;  //��ǰ�ؿ���

	HudLayer* _hud;  //hud��

	static GameLayer* nodeWithLevel(int level); //���ݹؿ�����ʼ��gamelayer
	bool initWithLevel(int level);				//���ݹؿ�����ʼ��gamelayer
	void exitLevel();

	void setEventState(EventState eventState);
	DamageNumber* getDamageNumber();
	HitEffect* getHitEffect();
protected:
	int _activeEnemies;     //��ǰ��Ծ�ĵ���
	float _viewPointOffset; //��Ұƫ��
	float _eventCenter;     //??

	cocos2d::CCArray *_battleEvents;      //��û�������¼�����
	cocos2d::CCDictionary *_currentEvent; //��ǰ�������¼�
	cocos2d::CCArray *_weapons;			  //�����б�
	EventState _eventState;				  //��Ϸ��ǰ��״̬

	cocos2d::CCArray *_damageNumbers;     //�������ʱ���ϵ�Ʈ��
	cocos2d::CCArray *_hitEffects;        //�������ʱ�Ĵ��Ч��

	cocos2d::CCArray *_mapObjects;		  //��ͼ�ϵ�����

	Boss *_boss;
private:
	cocos2d::CCTMXTiledMap* _tileMap;      //��Ϸ��ͼ
	cocos2d::CCSpriteBatchNode* _actors;   //��ɫ��������
	Hero *_hero;
	float _runDelay;   //�ܵ�ʱ����
	ActionDPadDirection _previousDirection; //�ϴ�D-pad���صķ���
	cocos2d::CCArray* _robots;
	cocos2d::CCArray* _brains;
};
#endif