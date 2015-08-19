#include "GameLayer.h"
#include "GameScene.h"
#include "ArtificialIntelligence.h"
#include "DamageNumber.h"
#include "HitEffect.h"
#include "TrashCan.h"
#include "TitleScene.h"
#include "Tools.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;

static GameLayer *s_GameLayer = NULL;

GameLayer::GameLayer()
{
	_battleEvents = NULL;
	_damageNumbers = NULL;
	_hitEffects = NULL;
	_robots = NULL;
	_tileMap = NULL;
	_brains = NULL;
	_weapons = NULL;
	_mapObjects = NULL;
	_hud = NULL;
	_viewPointOffset = 0.f;

	_activeEnemies = 0;
}

GameLayer::~GameLayer()
{
	_robots->removeAllObjects();
	CC_SAFE_RELEASE_NULL(this->_robots);

	_damageNumbers->removeAllObjects();
	CC_SAFE_RELEASE_NULL(this->_damageNumbers);

	_hitEffects->removeAllObjects();
	CC_SAFE_DELETE_ARRAY(this->_hitEffects);
	//CC_SAFE_RELEASE_NULL(this->_hitEffects);
	_brains->removeAllObjects();
	CC_SAFE_RELEASE_NULL(this->_brains);

	_battleEvents->removeAllObjects();
	CC_SAFE_RELEASE_NULL(this->_battleEvents);

	_weapons->removeAllObjects();
	CC_SAFE_RELEASE_NULL(this->_weapons);
	
	_mapObjects->removeAllObjects();
	CC_SAFE_RELEASE_NULL(this->_mapObjects);
}

void GameLayer::onExit()
{
	this->unscheduleUpdate();
}

GameLayer* GameLayer::inst()
{
	if (!s_GameLayer)
	{
		s_GameLayer = GameLayer::create();
	}
	return s_GameLayer;
}

bool GameLayer::init()
{
	bool bRet = false;
	CCLog("Game Init");
	do 
	{
		CC_BREAK_IF(!CCLayer::init());

		//����������ز�ʹ��������ͬһ��������������ʹ����CCSpriteBatchNode����������ֻ��Ⱦһ�Σ��������Ⱦ��Ч�ʣ�ע����3.0beta֮���Ƴ����Զ����δ������Բ�����Ҫʹ��CCSpriteBatchNode��
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Images/sprites.plist");
		_actors = CCSpriteBatchNode::create("Images/sprites.pvr.ccz");

		_actors->getTexture()->setAliasTexParameters();
		this->addChild(_actors, -5);

		//this->initTileMap("Images/map_level1.tmx");
		this->loadLevel(0);
		this->initHero();
		this->initRobots();
		this->initBrains();
		this->initEffects();

		this->initTestRobot();

		this->scheduleUpdate();

		bRet = true;
	} while (0);

	return bRet;
}

void GameLayer::initTileMap(const char* mapName)
{
	_tileMap = CCTMXTiledMap::create(mapName);
	CCObject *pObject = NULL;
	CCARRAY_FOREACH(_tileMap->getChildren(), pObject)
	{
		CCTMXLayer* child = (CCTMXLayer*)pObject;
		child->getTexture()->setAliasTexParameters();
	}
	this->addChild(_tileMap, -6);
}

void GameLayer::initHero()
 {
	this->_hero = Hero::create();
	_hero->_delegate = this;

	_actors->addChild(_hero, 3);
	_hero->setScale(kScaleFactor);
	_hero->setPosition(ccp(-_hero->getCenterToSides(), 80*kPointFactor));
	_hero->setDesiredPosition(_hero->getPosition());
	_hero->setGroundPosition(_hero->getPosition());
	//_hero->idle();

	_actors->addChild(_hero->getShadow(), 1);         //���Ӱ��
	float scale = _hero->getShadow()->getScale();
	_hero->getShadow()->setScale(scale * kScaleFactor);
	
}

void GameLayer::initRobots()
{
	int robotCount = 50;
	this->_robots = CCArray::createWithCapacity(robotCount);
	
	for (int i=0; i<robotCount; i++)
	{
		Robot *robot = Robot::create();
		robot->_delegate = this;

		_actors->addChild(robot->getShadow());
		_actors->addChild(robot->getSmoke());
		_actors->addChild(robot);
		_actors->addChild(robot->getBelt());
		_robots->addObject(robot);

		robot->setScale( robot->getScale() * kScaleFactor);
		CCSprite* shadow = robot->getShadow();
		shadow->setVisible(false);
		shadow->setScale(shadow->getScale()*kScaleFactor);
		robot->setShadow(shadow);

		robot->setPosition(OFFSCREEN);//�������󣬱����õ������������ֱ��������
		robot->_groundPosition = robot->getPosition();
		robot->setDesiredPosition(robot->getPosition());
		robot->setVisible(false);//����Ҫ����Ϊ���ɼ�����������Ͳ���Ҫȥ��Ⱦ����
		robot->setColorSet(kColorRandom);
		robot->_actionState = kActionStateNone;

		/*
		���������еĻ�����Զ������Ϊ�㲻����50������������׷��Ӣ�ۣ�������ǵ�����Ҫ��ʱ������������ݵ�ǰ��battle event
		*/


		/*
		//�����˳��ֵķ�Χ������Ļ�м䵽�������ұ�
		int minX = SCREEN.width + robot->getCenterToSides();
		int maxX = _tileMap->getMapSize().width * _tileMap->getTileSize().width * kPointFactor - robot->getCenterToSides();
		int minY = robot->getCenterToBottom();
		int maxY = 3 * _tileMap->getTileSize().height * kPointFactor + robot->getCenterToBottom();
		robot->setScaleX(-kScaleFactor);
		robot->setScaleY(kScaleFactor);
		CCSprite* shadow = robot->getShadow();
		shadow->setScale(kScaleFactor);
		robot->setShadow(shadow);
		robot->setPosition(ccp(random_range(minX, maxX), random_range(minY, maxY)));
		robot->_groundPosition = robot->getPosition();
		robot->setGroundPosition(robot->_groundPosition);
		robot->setDesiredPosition(robot->getPosition());
		robot->idle();
		robot->setColorSet(kColorRandom);
		*/
	}
	this->_robots->retain();
}

void GameLayer::initTestRobot()
{
	Robot *robot = Robot::create();
	robot->_delegate = this;

	_actors->addChild(robot->getShadow());
	_actors->addChild(robot->getSmoke());
	_actors->addChild(robot);
	_actors->addChild(robot->getBelt());
	_robots->addObject(robot);

	robot->setScale( robot->getScale() * kScaleFactor);
	CCSprite* shadow = robot->getShadow();
	shadow->setVisible(true);
	shadow->setScale(shadow->getScale()*kScaleFactor);
	robot->setShadow(shadow);

	CCPoint pos = ccp(150, 100);
	robot->setPosition(pos);
	robot->_groundPosition = robot->getPosition();
	robot->setDesiredPosition(robot->getPosition());
	robot->setVisible(true);
	robot->setColorSet(kColorRandom);
	robot->idle();
}
void GameLayer::initBrains()
{
	this->_brains = CCArray::createWithCapacity(_robots->count()+1);
	this->_brains->retain();

	ArtificialIntelligence *brainBoss = ArtificialIntelligence::aiWithControlledSprite(_boss, _hero);
	_brains->addObject(brainBoss);

	ArtificialIntelligence *brain = NULL;
	Robot *robot = NULL;
	CCObject *obj = NULL;
	CCARRAY_FOREACH(_robots, obj)
	{
		robot = (Robot *)obj;
		brain = ArtificialIntelligence::aiWithControlledSprite(robot, _hero);
		_brains->addObject(brain);
	}
}

void GameLayer::initEffects()
{
	int i;
	//�������
	this->_damageNumbers = CCArray::createWithCapacity(20);
	DamageNumber *number;
	for (i=0; i<20; i++)
	{
		number = DamageNumber::create();
		number->setVisible(false);
		number->setPosition(OFFSCREEN);
		this->addChild(number);
		_damageNumbers->addObject(number);
	}
	_damageNumbers->retain();

	//���Ч��
	this->_hitEffects = CCArray::createWithCapacity(20);
	HitEffect *effect;
	for (i=0; i<20; i++)
	{
		effect = HitEffect::create();
		effect->setVisible(false);
		effect->setScale(effect->getScale()*kScaleFactor);
		effect->setPosition(OFFSCREEN);
		_actors->addChild(effect);
		_hitEffects->addObject(effect);
	}
	_hitEffects->retain();
}

void GameLayer::initBossWithType(BossType boss)
{
	if (boss == kBossMohawk)
	{
		this->_boss = Boss::create();
		_boss->_delegate = this;
		_actors->addChild(_boss->_shadow);
		_boss->_shadow->setScale(_boss->_shadow->getScale()*kScaleFactor);
		_actors->addChild(_boss);
		_boss->setScale(_boss->getScale()*kScaleFactor);
		_boss->setVisible(true); //false
		_boss->setPosition(OFFSCREEN);
		_boss->setGroundPosition(OFFSCREEN);
		_boss->setDesiredPosition(OFFSCREEN);

		/*_boss->setPosition(ccp(100, 100));
		_boss->setGroundPosition(ccp(100, 100));
		_boss->setDesiredPosition(ccp(100, 100));*/
	}
}

void GameLayer::setHud(HudLayer* hud)
{
	_hud = hud;
	_hud->setHitPoints(_hero->getHitPoints(), _hero->getMaxHitPoints());
}

void GameLayer::didChangeDirectionTo(ActionDPad *actionDPad, ActionDPadDirection direction)
{
	if (_eventState != kEventStateScripted) //�ڽű��������ڲ��ò���Ӣ��
	{
		//CCLog("Change dir");
		//�趨һ���ж���

		CCPoint directionVector = this->vectorForDirection(direction); //������ĳ�����
		_hero->flipSpriteForVelocity(directionVector); //��ɫ�趨ת��
		//Ҫ��˫������ʱ�������ҷ���û�ı���û�������ҷ��������һ�����(���������ʵ����ȥ��),ִ���ܲ�������ֻ������Ҫ�ж��Ƿ������������Ϊ����û����Ӧ�ܵĶ�����
		if (!_hero->_weapon && _runDelay >0 && _previousDirection == direction && 
			(direction == kActionDPadDirectionRight || direction == kActionDPadDirectionLeft))
		{
			_hero->runWithDirection(directionVector);
		}
		else if (_hero->_actionState == kActionStateRun &&  //����Ӣ���ܶ��з���ı��ˣ���Ϊ��·
			abs(_previousDirection - direction)<=1)
		{
			_hero->moveWithDirection(directionVector);
		}
		else //û���ܵ�������˫�����ʱ�䲻������������
		{
			_hero->walkWithDirection(directionVector);
			_previousDirection = direction;
			_runDelay = 2000;//0.2f;
		}
	}
	
	//_hero->walkWithDirection(directionVector);
}
void GameLayer::isHoldingDirection(ActionDPad *actionDPad, ActionDPadDirection direction)
{
	if (_eventState != kEventStateScripted)
	{
		CCPoint directionVector = this->vectorForDirection(direction);
		_hero->moveWithDirection(directionVector);
	}
}
void GameLayer::actionDPadTouchEnded(ActionDPad *simpleDPad)
{
	 if (_eventState != kEventStateScripted &&(_hero->_actionState == kActionStateWalk || _hero->_actionState == kActionStateRun))
	 {
		 _hero->idle();
	 }
}

CCPoint GameLayer::vectorForDirection(ActionDPadDirection direction)
{
	float maxX = 1.0f;
	float maxY = 0.75f;

	switch (direction)
	{
	case kActionDPadDirectionCenter:
		return CCPointZero;
	case kActionDPadDirectionUp:
		return ccp(0.0, maxY);
	case kActionDPadDirectionUpRight:
		return ccp(maxX, maxY);
	case kActionDPadDirectionRight:
		return ccp(maxX, 0.0);
	case kActionDPadDirectionDownRight:
		return ccp(maxX, -maxY);
	case kActionDPadDirectionDown:
		return ccp(0.0, -maxY);
	case kActionDPadDirectionDownLeft:
		return ccp(-maxX, -maxY);
	case kActionDPadDirectionLeft:
		return ccp(-maxX, 0.0);
	case kActionDPadDirectionUpLeft:
		return ccp(-maxX, maxY);
	default:
		return CCPointZero;
	}
}

void GameLayer::actionButtonWasPressed(ActionButton* actionButton)
{
	if (_eventState != kEventStateScripted)
	{
		if (actionButton->getTag() == kTagButtonA) //���˹�����
		{
			bool pickedUpWeapon = false; //�Ƿ���������
			if (!_hero->_weapon)
			{
				Weapon *weapon = NULL;
				CCObject *objWea = NULL;
				CCARRAY_FOREACH(_weapons, objWea)
				{
					weapon = (Weapon *)objWea;
					if (weapon->_weaponState == kWeaponStateUnequipped)//����Ҫ��ж�µ�
					{
						if (this->collisionBetweenPlayer(_hero, weapon))//��ײ����
						{
							pickedUpWeapon = _hero->pickUpWeapon(weapon);//ʰȡ
							_actors->reorderChild(weapon, _hero->getZOrder()+1);
							//int orderW = weapon->getZOrder();
							//int orderH = _hero->getZOrder();
							break;
						}
					}
				}
			}

			//û��ʰȡ�������򹥻�
			if(!pickedUpWeapon)
				_hero->attack();
		}
		else if (actionButton->getTag() == kTagButtonB) //��������Ծ��
		{
			if (_hero->_weapon)
			{
				//ж������
				_hero->dropWeapon();
			}
			else
			{
				CCPoint directionVector = this->vectorForDirection(_hud->getDPad()->_direction);
				_hero->jumpRiseWithDirection(directionVector);
			}
			
		}
	}
	
}

void GameLayer::actionButtonIsHeld(ActionButton* actionButton)
{

}

void GameLayer::actionButtonWasReleased(ActionButton* actionButton)
{
	if (_eventState != kEventStateScripted)
	{
		if (actionButton->getTag()==kTagButtonB) //�ɿ�������Ծ��
		{
			_hero->jumpCutoff();
		}
	}
}

void GameLayer::update(float dt)
{
	_hero->update(dt);

	CCObject *objWeapon = NULL;
	Weapon *weapon = NULL;
	CCARRAY_FOREACH(_weapons, objWeapon)
	{
		weapon = (Weapon *)objWeapon;
		weapon->update(dt);
	}

	if (_boss)
	{
		_boss->update(dt);
	}

	//���»����˵�λ��
	Robot *robot = NULL;
	CCObject* objRob = NULL;
	CCARRAY_FOREACH(_robots, objRob)
	{
		robot = (Robot *)objRob;
		robot->update(dt);
	}

	this->updatePositions();

	//�����˹�����
	ArtificialIntelligence *brain = NULL;
	CCObject *obj = NULL;
	CCARRAY_FOREACH(_brains, obj)
	{
		brain = (ArtificialIntelligence *)obj;
		brain->update(dt);
	}

	this->reorderActors();
	//�ܲ����
	if (_runDelay > 0)
	{	_runDelay -= dt;
	}
	//this->setViewpointCenter(_hero->getPosition());
	//ֻ�е���������ʱ��������Ÿ�������ƶ���ս������ʱ�����������ƶ�
	if(_eventState == kEventStateFreeWalk || _eventState == kEventStateScripted)
		this->setViewpointCenter(_hero->getPosition());

	this->updateEvent();

	//���ϵĸ�����Ұƫ�ƣ�ʹ���ӽ�ƽ�����ƶ�
	if (_viewPointOffset < 0)
	{
		_viewPointOffset += SCREEN.width * dt;
		if(_viewPointOffset >= 0)
			_viewPointOffset = 0;
	}
	else if (_viewPointOffset > 0)
	{
		_viewPointOffset -= SCREEN.width * dt;
		if (_viewPointOffset <= 0)
		{
			_viewPointOffset = 0;
		}
	}
}

void GameLayer::updatePositions()
{
	float mapWidth = _tileMap->getMapSize().width * _tileMap->getTileSize().width * kPointFactor; //��ȡ��ͼʵ�ʴ�С
	float floorHeight = 3 * _tileMap->getTileSize().height * kPointFactor; //��ͼ�ϵĵذ�(��ҿ��ߵķ�Χ)ʵ�ʾ��������������� ������İ취

	float posX, posY;

	if (_hero->_actionState > kActionStateNone)
	{
		this->objectCollisionsForSprite(_hero); //����Ӣ�������ӵ���ײ

		//�������������״̬������ж���Χ����ǰһ��
		if (_eventState == kEventStateFreeWalk)
		{
			//�˴��ж��൱���ӣ��������ֶ�д�����ж���䣬��ɲ���Ū����ô��ɬ�Ѷ�
			//���ߵ���˼�ǣ�posX��λ�õ�Ȼ��Ҫȡhero��position(_hero->getDesiredPosition().x),���ֲ��ܳ������ұ߽磺mapWidth - _hero->feetCollisionRect().size.width/2
			//ͬʱ��ɫλ����С�ֲ���С����߽�:_hero->feetCollisionRect().size.width/2, Ȼ�����MIN��MAX������������ʵ����һ�������ɶ��������ж��ķ���
			/*
			��ʵҲ�ܼ򵥣���ɫλ����С����С����߽���ô��ʹ��MAX��������һ����������߽磬����������һ��ֵ�Ƕ��٣�MAX�Ľ��������С����߽�
			ͬʱ���ܳ������ұ߽��ʹ��MIN��������һ���������ұ߽磬����������һ��ֵ���٣�MIN��ֵ����������ұ߽�
			�����߽�ϾͿ��Ա�֤posX��ֵ��С����߽��Ҳ������ұ߽�
			*/
			posX = MIN(mapWidth - _hero->feetCollisionRect().size.width/2, 
				MAX(_hero->feetCollisionRect().size.width/2, _hero->getDesiredPosition().x));
			posY = MIN(floorHeight + (_hero->getCenterToBottom() - _hero->feetCollisionRect().size.height), 
				MAX(_hero->getCenterToBottom(), _hero->getDesiredPosition().y));

			_hero->setGroundPosition(ccp(posX, posY));
			_hero->setPosition(ccp(_hero->_groundPosition.x, _hero->_groundPosition.y + _hero->getJumpHeight()));
		}
		// ��ս״̬ʱ������ж���Χ���̶���eventCenter��Χ֮�ڣ���ֻ��ս�·�����Χ����eventCenterΪ���ģ����Ұ����Ļ��Χ���ɶ�
		else if (_eventState == kEventStateBattle)
		{
			posX = MIN(MIN(mapWidth - _hero->feetCollisionRect().size.width/2, _eventCenter + CENTER.x -
				_hero->feetCollisionRect().size.width/2), MAX(_eventCenter - CENTER.x + _hero->feetCollisionRect().size.width/2, 
				_hero->getDesiredPosition().x));
			posY = MIN(floorHeight + (_hero->getCenterToBottom() - _hero->feetCollisionRect().size.height), MAX(_hero->getCenterToBottom(), 
				_hero->getDesiredPosition().y));
			_hero->setGroundPosition(ccp(posX, posY));
			_hero->setPosition(ccp(_hero->_groundPosition.x, _hero->_groundPosition.y + _hero->getJumpHeight()));
		}
	}

	//���û����˵�λ��
	Robot* robot = NULL;
	CCObject* obj = NULL;
	CCARRAY_FOREACH(_robots, obj)
	{
		robot = (Robot *)obj;
		if (robot->_actionState > kActionStateNone)
		{
			this->objectCollisionsForSprite(robot); //��������������ӵ���ײ
			//��һ���㲻���ƻ����˵�x�����ꡣ�����˱���������Ļ���⣬���ǺϺ��߼��ģ���Ϊ���ǳ����ڵ�ͼ��
			//��Σ���һ������������Ӣ�۵ľ��������Ļ�Ŀ�������Ȼ������˱�ò��ɼ�and��λ
			posY = MIN(floorHeight + (robot->getCenterToBottom() -robot->feetCollisionRect().size.height), MAX(robot->getCenterToBottom(), 
				robot->getDesiredPosition().y));
			robot->setGroundPosition(ccp(robot->getDesiredPosition().x, posY));
			robot->setPosition(ccp(robot->_groundPosition.x, 
				robot->_groundPosition.y + robot->getJumpHeight()));

			if (robot->_actionState == kActionStateDead && 
				_hero->_groundPosition.x - robot->_groundPosition.x >= SCREEN.width + 
				robot->getContentSize().width/2 * kScaleFactor)
			{
				robot->setVisible(false);
				robot->reset();
			}
		}
	}
	
	if (_boss && _boss->_actionState>kActionStateNone)
	{
		this->objectCollisionsForSprite(_boss);
		posY = MIN(floorHeight+(_boss->getCenterToBottom() - _boss->feetCollisionRect().size.height), 
			MAX(_boss->getCenterToBottom(), _boss->getDesiredPosition().y));
		_boss->setGroundPosition(ccp(_boss->getDesiredPosition().x, posY));
		_boss->setPosition(ccp(_boss->_groundPosition.x, _boss->_groundPosition.y + _boss->getJumpHeight()));
	}

	//��������λ��
	Weapon *weapon = NULL;
	CCObject *objWeapon = NULL;
	CCPoint posWeapon;
	CCARRAY_FOREACH(_weapons, objWeapon)
	{
		weapon = (Weapon *)objWeapon;
		if (weapon->_weaponState > kWeaponStateEquipped)
		{
			posWeapon = ccp(weapon->_groundPosition.x, weapon->_groundPosition.y + weapon->_jumpHeight);
			weapon->setPosition(posWeapon);
		}
	}
}

void GameLayer::setViewpointCenter(cocos2d::CCPoint position)
{
	float mapWidth = _tileMap->getMapSize().width * _tileMap->getTileSize().width * kPointFactor;
	float mapHeight = _tileMap->getMapSize().height * _tileMap->getTileSize().height * kPointFactor;
	float x = MAX(position.x, CENTER.x);
	float y = MAX(position.y, CENTER.y);
	x = MIN( x, (mapWidth-CENTER.x) );
	y = MIN( y, (mapHeight-CENTER.y) );

	CCPoint actualPosition = ccp(x, y);

	CCPoint viewPoint = ccpSub(CENTER, actualPosition);
	//this->setPosition(viewPoint);
	this->setPosition(ccp(viewPoint.x + _viewPointOffset, viewPoint.y));//Ϊ��ƽ���Ĺ��ɵ�Ӣ��Ϊ���ĵ��ӽǣ�������ͻȻ�ƶ�
}

void GameLayer::reorderActors()
{
	int spriteZ = 0;

	//�ȸ��µ���������Zorder����ֹ�������Ӣ���ֲ�����ʱzorder������ͻ
	CCObject *obj2 = NULL;
	Weapon *objWeapon = NULL;
	CCARRAY_FOREACH(_weapons, obj2)
	{
		objWeapon = (Weapon *)obj2;
		spriteZ = this->getZFromYPosition(objWeapon->_shadow->getPositionY());
		_actors->reorderChild(objWeapon->_shadow, spriteZ);
		_actors->reorderChild(objWeapon, spriteZ);
	}

	spriteZ = this->getZFromYPosition(_hero->getShadow()->getPositionY()); //hero��zorder
	//����Ӣ�۵�zorder
	_actors->reorderChild(_hero->getShadow(), spriteZ);
	_actors->reorderChild(_hero, spriteZ);

	if(_hero->_weapon) //������ʱ�����������ϲ㣬�����ƶ��ײ�ʱ����ִ���
		_actors->reorderChild(_hero->_weapon, _hero->getZOrder()+1);

	

	Robot *robot = NULL;
	CCObject *obj = NULL;
	CCARRAY_FOREACH(_robots, obj)
	{
		robot = (Robot *)obj;
		spriteZ = this->getZFromYPosition(robot->getShadow()->getPositionY());
		_actors->reorderChild(robot->getShadow(), spriteZ);
		_actors->reorderChild(robot->getSmoke(), spriteZ);
		_actors->reorderChild(robot->getBelt(), spriteZ);
		_actors->reorderChild(robot, spriteZ);
	}

	if (_boss)
	{
		spriteZ = this->getZFromYPosition(_boss->getShadow()->getPositionY());
		_actors->reorderChild(_boss->_shadow, spriteZ);
		_actors->reorderChild(_boss, spriteZ);
	}
	
	MapObject *objMap = NULL;
	CCObject *objM = NULL;
	CCARRAY_FOREACH(_mapObjects, objM)
	{
		objMap = (MapObject *)objM;
		spriteZ = this->getZFromYPosition(objMap->collisionRect().origin.y + objMap->collisionRect().size.height/2 );
		_actors->reorderChild(objMap, spriteZ);
	}
}

int GameLayer::getZFromYPosition(float yPosition)
{
	//y����ֵԽС�����ص�zֵԽ����ʾҲ��Խ��ǰ���ұ�֤��zֵ������ֵ
	return (int)((_tileMap->getMapSize().height * _tileMap->getTileSize().height * kPointFactor) - yPosition);
}

void GameLayer::draw()
{
	CCLayer::draw();

#if DRAW_DEBUG_SHAPES
	this->drawShapesForActionSprite(_hero);

	CCObject *obj;
	ActionSprite *actionSprite;
	CCARRAY_FOREACH(_robots, obj)
	{
		actionSprite = (ActionSprite *)obj;
		this->drawShapesForActionSprite(actionSprite);
	}
#endif
}

void GameLayer::drawShapesForActionSprite(ActionSprite* sprite)
{
	if (sprite->isVisible())
	{
		int i;
		
		//�������Բ
		ccDrawColor4B(0, 0, 255, 255);
		ccDrawCircle(sprite->getPosition(), sprite->_detectionRadius, 0, 16, false, 1.0f, 1.0f);

		ccDrawColor4B(0, 255, 0, 255);

		//���ƽӴ�Բ
		for (i=0; i<sprite->_contactPointCount; i++)
		{
			ccDrawCircle(sprite->_contactPoints[i].position, sprite->_contactPoints[i].radius, 0, 8, false);
		}

		//�����ж�Բ
		ccDrawColor4B(255, 0,0, 255);
		for (i=0; i<sprite->_attackPointCount; i++)
		{
			ccDrawCircle(sprite->_attackPoints[i].position, sprite->_attackPoints[i].radius, 0, 8, false);
		}

		//���ƽŲ����ο�
		ccDrawColor4B(255, 255, 0, 255);
		ccDrawRect(sprite->feetCollisionRect().origin,
			ccp(sprite->feetCollisionRect().origin.x + 
			sprite->feetCollisionRect().size.width,
			sprite->feetCollisionRect().origin.y +
			sprite->feetCollisionRect().size.height));
	}
}

bool GameLayer::actionSpriteDidDie(ActionSprite *actionSprite)
{
	if (actionSprite == _hero)
	{
		_hud->setHitPoints(0.f, _hero->getMaxHitPoints());
		_hud->showMessage("GAME OVER", COLOR_LOWHP);
		CCSequence* seq = CCSequence::create(CCDelayTime::create(2.f), CCCallFunc::create(this, callfunc_selector(GameLayer::gotoMainUI)), NULL);
		this->runAction(seq);
	}
	else
	{
		//���˹ҵ��ˣ�������1
		_activeEnemies--;
		return true;
	}
	return false;
}

void GameLayer::gotoMainUI()
{
	CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(1.f, TitleScene::create()));
}

bool GameLayer::actionSpriteDidAttack(ActionSprite *actionSprite)
{
	bool didHit = false;  //��û�д򵽱���
	CCPoint attackPosition; //���Point�������洢��ײ���
	if (actionSprite == _hero) //�����Ӣ�ۣ������ÿ�������˵Ĺ�����ײ
	{
		
		Robot *robot;
		CCObject* obj;
		CCARRAY_FOREACH(_robots, obj)
		{
			//2
			robot = (Robot *)obj;
			if (robot->_actionState < kActionStateKnockedOut && 
				robot->_actionState != kActionStateNone)
			{
				if (this->collisionBetweenAttacker(_hero, robot, &attackPosition))
				{
					bool bShowEffect = true; //�Ƿ��д����Ч
					
					//���������ˣ�Ʈ���˺���
					DamageNumber* damageNumber = this->getDamageNumber();

					//ע��hurtWithDamage�����غ��������������ܻ��ĵ����ǻ���˻��Ǳ����ڵ�

					if (_hero->_actionState == kActionStateJumpAttack) //��Ծ����
					{
						SimpleAudioEngine::sharedEngine()->playEffect(EFFECT_HIT1_WAV); //���Ŵ����Ч

						robot->knockoutWithDamage(_hero->getJumpAttackDamage(), ccp(_hero->_directionX, 0));
						damageNumber->showWithValue(_hero->getJumpAttackDamage(), COLOR_MIDHP,robot->getPosition());
						bShowEffect = false;
					}
					else if(_hero->_actionState == kActionStateRunAttack) //�ܲ�����
					{
						SimpleAudioEngine::sharedEngine()->playEffect(EFFECT_HIT0_WAV);
						robot->knockoutWithDamage(_hero->getRunAttackDamage(), ccp(_hero->_directionX, 0));
						damageNumber->showWithValue(_hero->getRunAttackDamage(),COLOR_MIDHP, robot->getPosition());
					}
					else if(_hero->_actionState == kActionStateAttackThree)
					{
						SimpleAudioEngine::sharedEngine()->playEffect(EFFECT_HIT1_WAV);
						robot->knockoutWithDamage(_hero->_attackThreeDamage, ccp(_hero->_directionX, 0));
						damageNumber->showWithValue(_hero->_attackThreeDamage,COLOR_MIDHP,robot->getPosition());
						//bShowEffect = false;
					}
					else if(_hero->_actionState == kActionStateAttackTwo)
					{
						//���һ���ܻ�������ͬ
						SimpleAudioEngine::sharedEngine()->playEffect(EFFECT_HIT0_WAV);
						robot->hurtWithDamage(_hero->_attackTwoDamage, _hero->getAttackForce(),ccp(_hero->_directionX, 0));
						damageNumber->showWithValue(_hero->_attackTwoDamage,COLOR_MIDHP, robot->getPosition());
					}
					else
					{
						SimpleAudioEngine::sharedEngine()->playEffect(EFFECT_HIT0_WAV);
						robot->hurtWithDamage(_hero->getAttackDamage(), _hero->getAttackForce(), ccp(_hero->_directionX, 0.0f));
						damageNumber->showWithValue(_hero->getAttackDamage(),COLOR_MIDHP, robot->getPosition());
					}

					didHit = true; //��ʾ�򵽻�������

					if (bShowEffect) //���Ŵ����Ч
					{
						HitEffect *hitEffect = this->getHitEffect();
						_actors->reorderChild(hitEffect, MAX(robot->getZOrder(), _hero->getZOrder())+1);
						hitEffect->showEffectAtPosition(attackPosition);
					}
				}
			}
		}
		//add this new if block
		if (_boss && _boss->_actionState < kActionStateKnockedOut && _boss->_actionState != kActionStateNone)
		{
			if (this->collisionBetweenAttacker(_hero, _boss, &attackPosition))
			{
				bool showEffect = true;
				DamageNumber *damageNumber = this->getDamageNumber();
				if (_hero->_actionState == kActionStateJumpAttack)
				{
					SimpleAudioEngine::sharedEngine()->playEffect(EFFECT_HIT1_WAV);
					_boss->hurtWithDamage(_hero->getJumpAttackDamage(), _hero->getAttackForce(), ccp(_hero->_directionX, 0.f));
					damageNumber->showWithValue(_hero->getJumpAttackDamage(),COLOR_MIDHP, _boss->getPosition());
					showEffect = false;
				}
				else if (_hero->_actionState == kActionStateRunAttack)
				{
					SimpleAudioEngine::sharedEngine()->playEffect(EFFECT_HIT0_WAV);
					_boss->hurtWithDamage(_hero->getRunAttackDamage(), _hero->getAttackForce(), ccp(_hero->_directionX, 0.f));
					damageNumber->showWithValue(_hero->getRunAttackDamage(), COLOR_MIDHP, _boss->getPosition());
				}
				else if (_hero->_actionState == kActionStateAttackThree)
				{
					SimpleAudioEngine::sharedEngine()->playEffect(EFFECT_HIT1_WAV);
					_boss->hurtWithDamage(_hero->_attackThreeDamage, _hero->getAttackForce(), ccp(_hero->_directionX, 0.f));
					damageNumber->showWithValue(_hero->_attackThreeDamage, COLOR_MIDHP, _boss->getPosition());
				}
				else if (_hero->_actionState == kActionStateAttackTwo)
				{
					SimpleAudioEngine::sharedEngine()->playEffect(EFFECT_HIT0_WAV);
					_boss->hurtWithDamage(_hero->_attackTwoDamage, _hero->getAttackForce(), ccp(_hero->_directionX, 0.f));
					damageNumber->showWithValue(_hero->_attackTwoDamage, COLOR_MIDHP, _boss->getPosition());
				}
				else
				{
					SimpleAudioEngine::sharedEngine()->playEffect(EFFECT_HIT0_WAV);
					_boss->hurtWithDamage(_hero->getAttackDamage(), 0, ccp(_hero->_directionX, 0.f));
					damageNumber->showWithValue(_hero->getAttackDamage(), COLOR_MIDHP, _boss->getPosition());
				}
				didHit = true;

				if (showEffect)
				{
					HitEffect *hitEffect = this->getHitEffect();
					_actors->reorderChild(hitEffect, MAX(_boss->getZOrder(), _hero->getZOrder())+1);
					hitEffect->showEffectAtPosition(attackPosition);
				}
			}
		}

		//�ж����������ײ
		MapObject *mapObject = NULL;
		CCObject *object = NULL;
		CCARRAY_FOREACH(_mapObjects, object)
		{
			mapObject = (MapObject *)object;
			if (this->collisionBetweenAttacker(_hero, mapObject, &attackPosition)) //���������������
			{
				HitEffect *hitEffect = this->getHitEffect();
				_actors->reorderChild(hitEffect, MAX(mapObject->getZOrder(), _hero->getZOrder())+1);
				hitEffect->showEffectAtPosition(attackPosition);

				if (mapObject->_objectState != kObjectStateDestroyed)
				{
					SimpleAudioEngine::sharedEngine()->playEffect(EFFECT_HIT1_WAV);
					mapObject->destroyed();
					Weapon *weapon = this->getWeapon();
					weapon->droppedFrom(mapObject->getContentSize().height/2*kScaleFactor, 
						ccp(mapObject->getPositionX(), mapObject->getPositionY() - mapObject->getContentSize().height/2*kScaleFactor));
					weapon->setVisible(true);
				}
				else
				{
					SimpleAudioEngine::sharedEngine()->playEffect(EFFECT_HIT0_WAV);
				}
			}
		}
		return didHit;
	}
	else if (actionSprite == _boss)
	{
		if (_hero->_actionState < kActionStateKnockedOut && _hero->_actionState != kActionStateNone)
		{
			CCPoint attackPosition;
			if (this->collisionBetweenAttacker(_boss, _hero, &attackPosition))
			{
				SimpleAudioEngine::sharedEngine()->playEffect(EFFECT_HIT1_WAV);
				_hero->knockoutWithDamage(_boss->getAttackDamage(), ccp(actionSprite->_directionX, 0.f));
				_hud->setHitPoints(_hero->getHitPoints(), _hero->getMaxHitPoints());//����Ѫ��
				didHit = true;

				DamageNumber *damageNumber = this->getDamageNumber();
				damageNumber->showWithValue(_boss->getAttackDamage(),COLOR_MIDHP, _hero->getPosition());

				HitEffect *hitEffect = this->getHitEffect();
				_actors->reorderChild(hitEffect, MAX(_boss->getZOrder(), _hero->getZOrder()+1));
				hitEffect->showEffectAtPosition(attackPosition);
			}
		}
	}
	else   //����ǻ����ˣ�����Ƿ����Ӣ��
	{
		if(_hero->_actionState < kActionStateKnockedOut && 
			_hero->_actionState != kActionStateNone && _hero->_isInvincible==false)
		{
			CCPoint attackPosition;
			if (this->collisionBetweenAttacker(actionSprite, _hero, &attackPosition))
			{
				SimpleAudioEngine::sharedEngine()->playEffect(EFFECT_HIT0_WAV);
				_hero->hurtWithDamage(actionSprite->getAttackDamage(), actionSprite->getAttackForce(), ccp(actionSprite->_directionX, 0.0f));
				_hud->setHitPoints(_hero->getHitPoints(), _hero->getMaxHitPoints()); //����Ѫ��
				didHit = true; //����Ӣ��

				DamageNumber *damageNumber = this->getDamageNumber();
				damageNumber->showWithValue(actionSprite->getAttackDamage(), COLOR_LOWHP, _hero->getPosition());

				HitEffect *hitEffect = this->getHitEffect();
				_actors->reorderChild(hitEffect, MAX(actionSprite->getZOrder(), _hero->getZOrder())+1);
				hitEffect->showEffectAtPosition(attackPosition);
			}
		}
	}

	
	return didHit;
}

//����˵���ײ���
bool GameLayer::collisionBetweenAttacker(ActionSprite* attacker, ActionSprite* target, cocos2d::CCPoint* position)
{
	//first; ����Ƿ���ͬһƽ���� ����ɫ��y�����Ƿ�ӽ�
	float planeDist = attacker->getShadow()->getPositionY() - target->getShadow()->getPositionY();

	
	if (fabsf(planeDist) <= kPlaneHeight) //�ж�y��������Ƿ���һ��ƽ������������
	{
		int i,j;
		//̽��߽磨������������뾶֮�ͣ�
		float combinedRadius = attacker->_detectionRadius + target->_detectionRadius;

		//initial detection �жϹ������뱻�������Ƿ������̽�ⷶΧ��������Ч�ʸߣ�
		if (ccpDistanceSQ(attacker->getPosition(), target->getPosition()) <= combinedRadius * combinedRadius)
		{
			//�ཻ�ˣ�������һ�����

			int attackPointCount = attacker->_attackPointCount;
			int contactPointCount = target->_contactPointCount;

			ContactPoint attackPoint, contactPoint;
			//secondary detection
			//��鹥�������еĹ������뱻�����ߵ���ײ���Ƿ��нӴ�
			for (i=0; i<attackPointCount; i++)
			{
				attackPoint = attacker->_attackPoints[i];
				for (j=0; j<contactPointCount; j++)
				{
					contactPoint = target->_contactPoints[j];
					combinedRadius = attackPoint.radius + contactPoint.radius;
					//�������ײ��Χ
					if (ccpDistanceSQ(attackPoint.position, contactPoint.position) <= combinedRadius * combinedRadius)
					{
						//������ײ��λ��
						position->x = attackPoint.position.x;
						position->y = attackPoint.position.y;
						return true;
					}
				}
			}
		}
	}

	//ΪʲôҪԲ�ζ���ʹ�þ���������ײ������Ϊ���μ��Ƚ��鷳Ҫ��ע�ܶ�㣨156ҳ����������ֻ��Ҫ��ע�������������
	return false;
}

bool GameLayer::collisionBetweenAttacker(ActionSprite* attacker, MapObject* object, cocos2d::CCPoint* position)
{
	//1 ����Ƿ���ͬһƽ��
	float objectBottom = object->collisionRect().origin.y;  //����ĵײ��߶�
	float objectTop = objectBottom + object->collisionRect().size.height; //����Ķ����߶�
	float attackerBottom = attacker->feetCollisionRect().origin.y;  //�����ߵ��㲿�߶�
	float attackerTop = attackerBottom + attacker->feetCollisionRect().size.height;  //�����ߵ��㲿�������ĸ߶�
	//�жϹ����ߵ��㲿���η�Χ�Ƿ���object�ķ�Χ֮��
	if ((attackerBottom>objectBottom && attackerBottom<objectTop) || (attackerTop>objectBottom && attackerTop<objectTop))
	{
		int i, j;
		float combinedRadius = attacker->_detectionRadius + object->_detectionRadius;

		//�жϹ���������������Ƿ������ߵ���ⷶΧ֮��
		if (ccpDistanceSQ(attacker->getPosition(), object->getPosition()) <= combinedRadius*combinedRadius)
		{
			int attackPointCount = attacker->_attackPointCount;
			int contactPointCount = object->_contactPointCount;

			ContactPoint attackPoint, contactPoint;
			//���ÿ�������㣬�����ӵ�ÿ���Ӵ��ж��������ײ����ж�
			for (i=0; i<attackPointCount; i++)
			{
				attackPoint = attacker->_attackPoints[i];
				for (j=0; j<contactPointCount; j++)
				{
					contactPoint = object->_contactPoints[j];
					combinedRadius = attackPoint.radius + contactPoint.radius;
					//��⹥������Ӵ����Ƿ���ײ,������빻��������true
					if (ccpDistanceSQ(attackPoint.position, contactPoint.position)<=combinedRadius*combinedRadius)
					{
						position->x = attackPoint.position.x;
						position->y = attackPoint.position.y;
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool GameLayer::collisionBetweenPlayer(ActionSprite* player, Weapon* weapon)
{
	float planeDist = player->getShadow()->getPositionY() - weapon->_shadow->getPositionY();

	if (fabsf(planeDist)<=kPlaneHeight)
	{
		float combineRadius = player->_detectionRadius + weapon->_detectionRadius;
		int i;

		//̽����������ҵľ����Ƿ�����ⷶΧ��
		if (ccpDistanceSQ(player->getPosition(), weapon->getPosition()) <= combineRadius * combineRadius)
		{
			int contactPointCount = player->_contactPointCount;
			ContactPoint contactPoint;

			//�ж��������ÿһ���Ӵ��ж����Ƿ��������Ĵ�����Χ��
			for (i=0; i<contactPointCount; i++)
			{
				contactPoint = player->_contactPoints[i];
				combineRadius = contactPoint.radius + weapon->_detectionRadius;
				if (ccpDistanceSQ(contactPoint.position, weapon->getPosition()) <= combineRadius * combineRadius)
				{
					return true;  //��һ���Ӵ��ˣ��ͷ���true
				}
			}
		}
	}
	return false;
}

void GameLayer::loadLevel(int level)
{
	//��ȡLevels.plist������·��
	std::string levelsPlist = CCFileUtils::sharedFileUtils()->fullPathForFilename("Levels.plist");
	//��Levels.plist������ת��Ϊ����
	CCArray* levelArray = CCArray::createWithContentsOfFile(levelsPlist.c_str());
	//��ȡ�����еĵ�һ�����飬���ǵ�һ������
	CCDictionary *levelData = CCDictionary::createWithDictionary((CCDictionary *)(levelArray->objectAtIndex(level)));

	CCString *tileMap = (CCString *)levelData->objectForKey("TileMap");
	this->initTileMap(tileMap->getCString());

	_battleEvents = CCArray::createWithArray((CCArray *)levelData->objectForKey("BattleEvents"));
	_battleEvents->retain();
	
	_totalLevel = levelArray->count();
	_currentLevel = level;

	//����boss
	int boss = levelData->valueForKey("BossType")->intValue();
	this->initBossWithType(BossType(boss));
}

void GameLayer::spawnEnemies(cocos2d::CCArray *enemies, float origin)
{
	CCDictionary *enemyData = NULL;
	Robot *robot;

	int type, color;
	float row;
	float offset;

	//��ѭС������
	CCObject *obj  = NULL;
	CCARRAY_FOREACH(enemies, obj)
	{
		enemyData = (CCDictionary *)obj;
		//��ȡ�У��У�ƫ�����ݣ�ȷ�����˵�������λ��
		row = enemyData->valueForKey("Row")->floatValue();
		type = enemyData->valueForKey("Type")->intValue();
		offset = enemyData->valueForKey("Offset")->floatValue();

		//���������С������(����BOSS���ͣ���Ŀǰû��)
		if (type == kEnemyRobot)
		{
			color = enemyData->valueForKey("Color")->intValue();

			//��ȡһ��δ�ù���robot����������initRobots�������Ѿ����д���
			CCARRAY_FOREACH(_robots, obj)
			{
				robot = (Robot *)obj;
				//��ʼ��ʱrobot��״̬ΪNone
				if (robot->_actionState == kActionStateNone)
				{
					robot->stopAllActions();
					robot->setVisible(false);
					//���ݻ����˵Ĳ��������λ��
					CCPoint pos = ccp(
						//CENTER.x + robot->getCenterToSides()��λ�������������ߵı߽�λ�ã����offsetΪ1�������ұߣ�-1���������
						origin+(offset * (CENTER.x + robot->getCenterToSides())), 
						//�ӻ����˵Ľŵ����ϼ��������еĸ߶�
						robot->getCenterToBottom() +_tileMap->getTileSize().height * row * kPointFactor
						);
					robot->setGroundPosition(pos); 
					robot->setPosition(robot->_groundPosition);
					robot->setDesiredPosition(robot->_groundPosition);
					robot->setColorSet((ColorSet)color);
					robot->idle();
					robot->setVisible(true);
					robot->getShadow()->setVisible(true);
					break;
				}
			}
		}
		else if (type == kEnemyBoss)
		{
			CCPoint pos = ccp(origin + (offset * (CENTER.x + _boss->getCenterToSides())), _boss->getCenterToBottom() +
				_tileMap->getTileSize().height * row * kPointFactor);
			//pos = _hero->getPosition();//ccp(100,100);
			_boss->setGroundPosition(pos);
			_boss->setPosition(pos);
			_boss->setDesiredPosition(_boss->_groundPosition);
			_boss->idle();
			_boss->setVisible(true);
		}
	}
}

void GameLayer::updateEvent()
{
	if (_eventState == kEventStateBattle && _activeEnemies <= 0)
	{
		//��ǰ��������

		//������Ұ
		float maxCenterX = _tileMap->getMapSize().width * _tileMap->getTileSize().width * kPointFactor - CENTER.x;
		float cameraX = MAX(MIN(_hero->getPosition().x, maxCenterX), CENTER.x);
		_viewPointOffset = cameraX - _eventCenter;

		//����������е�ս���¼������ˣ����˳�����
		if(_battleEvents->count()==0)
			this->exitLevel();
		else
			this->setEventState(kEventStateFreeWalk);//�л���ȥ
	}
	else if (_eventState == kEventStateFreeWalk)
	{
		this->cycleEvents();
	}
	else if (_eventState == kEventStateScripted) //add this
	{
		float exitX = _tileMap->getTileSize().width * _tileMap->getMapSize().width * kPointFactor + _hero->getCenterToSides();
		//Ӣ���߳��ؿ��߽磬������һ��
		if (_hero->getPositionX() >= exitX)
		{
			_eventState = kEventStateEnd;
			if (_currentLevel < _totalLevel - 1)
			{
				CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(1.f, GameScene::nodeWithLevel(_currentLevel+1)));
			}
			else //���йؿ����꣬������Ϸ
			{
				//end game
				_hud->showMessage("YOU WIN", COLOR_FULLHP);
				CCSequence* seq = CCSequence::create(CCDelayTime::create(2.f), CCCallFunc::create(this, callfunc_selector(GameLayer::gotoMainUI)), NULL);
				this->runAction(seq);
			}
		}
	}
}

void GameLayer::cycleEvents()
{
	CCDictionary *event = NULL;
	CCObject *obj = NULL; 
	int column;
	float tileWidth = _tileMap->getTileSize().width * kPointFactor;

	//����δ�����¼��ļ��ϣ���ȡÿ���¼��е�λ��ͬʱ����_eventCenterλ�ã��ж��ĸ�������Ļ����
	CCARRAY_FOREACH(_battleEvents, obj)
	{
		event = (CCDictionary *)obj;
		column = event->valueForKey("Column")->intValue();//����ߵ����к󼤻����
		float maxCenterX = _tileMap->getMapSize().width * _tileMap->getTileSize().width * kPointFactor - CENTER.x;
		float columnPosition = column * tileWidth - tileWidth/2;
		_eventCenter = MAX(MIN(columnPosition, maxCenterX), CENTER.x);

		if (_hero->getPosition().x >= _eventCenter)  //����߹���Ļ�볡�ˣ��������
		{
			_currentEvent = event;
			_eventState = kEventStateBattle;//���õ�ǰ�¼�״̬Ϊս��
			//_battleEvents = CCArray::createWithArray((CCArray *)levelData->objectForKey("BattleEvents"));
			CCArray *enemyData = CCArray::createWithArray((CCArray *)event->objectForKey("Enemies"));//��ȡ���˵����ݣ��������˵����ͣ�λ�ã���ɫ��
			_activeEnemies = enemyData->count();
			this->spawnEnemies(enemyData, _eventCenter);//��������
			this->setViewpointCenter(ccp(_eventCenter, _hero->getPositionY()));//�����ӽ�
			break;
		}
	}

	//��δ�����¼��������Ƴ���
	if (_eventState == kEventStateBattle)
	{
		_battleEvents->removeObject(_currentEvent);
	}
}

void GameLayer::onEnterTransitionDidFinish()
{
	CCLayer::onEnterTransitionDidFinish();

	_hud->displayLevel(_currentLevel+1); //��ʾ�ؿ���Ϣ

	//�ս���ؿ�ʱ��ɫ���Լ��߽����ģ�������ҿ���
	_eventState = kEventStateScripted;
	_hero->enterFrom(_hero->getPosition(), ccp(64.f, _hero->getPositionY()));

	//������ʱ������ʱ2sִ�У�ִ��1�Σ�ִ��triggerEvent�����л�״̬ΪkEventStateFreeWalk
	//this->schedule(schedule_selector(GameLayer::triggerEvent),0, 1, 2);
	CCSequence* seq = CCSequence::create(CCDelayTime::create(2.2f), CCCallFuncND::create(this, callfuncND_selector(GameLayer::triggerEvent),(void*)kEventStateFreeWalk), NULL);
	this->runAction(seq);
}

void GameLayer::triggerEvent(CCNode *pSender, void* data)
{
	int state = int(data);
	this->_eventState = (EventState)state;
}

GameLayer* GameLayer::nodeWithLevel(int level)
{
	GameLayer* pRet = new GameLayer();
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

bool GameLayer::initWithLevel(int level)
{
	bool bRet = false;

	do 
	{
		CC_BREAK_IF(!CCLayer::init());

		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Images/sprites.plist");
		_actors = CCSpriteBatchNode::create("Images/sprites.pvr.ccz");

		_actors->getTexture()->setAliasTexParameters();
		this->addChild(_actors, -5);

		//this->initTileMap("Images/map_level1.tmx");
		this->loadLevel(level);
		this->initHero();
		this->initRobots();
		this->initWeapons();
		this->initBrains();
		this->initEffects();
		this->initMapObjects();

		this->initTestRobot();

		this->scheduleUpdate();

		bRet = true;
	} while (0);

	return bRet;
}

void GameLayer::initWeapons()
{
	int i;
	this->_weapons = CCArray::createWithCapacity(3);
	Weapon *weapon;

	for (i=0; i<3; i++)
	{
		weapon = Gauntlets::create();
		weapon->setVisible(true);
		weapon->_shadow->setScale(weapon->_shadow->getScale()*kScaleFactor);
		//float maxX = _tileMap->getMapSize().width * _tileMap->getTileSize().width * kPointFactor;
		/*float maxX = 300* (i+1);
		float minY = weapon->_centerToBottom;
		float maxY = 3 * _tileMap->getTileSize().height * kPointFactor + weapon->_centerToBottom;
		weapon->_weaponState = kWeaponStateUnequipped;
		
		CCPoint pos = ccp(frandom_range(0, maxX), frandom_range(minY, maxY));
		weapon->setPosition(pos);
		weapon->setGroundPosition(pos);
		_actors->addChild(weapon->_shadow);
		_actors->addChild(weapon);
		_weapons->addObject(weapon);*/
		weapon->setScale(weapon->getScale()*kScaleFactor);
		weapon->_groundPosition = OFFSCREEN;
		_actors->addChild(weapon->_shadow);
		_actors->addChild(weapon);
		_weapons->addObject(weapon);
	}
	_weapons->retain();
}

void GameLayer::initMapObjects()
{
	CCTMXObjectGroup *objectGroup = _tileMap->objectGroupNamed("Objects");    //���Ȼ�ȡ��ͼ��object��
	this->_mapObjects = CCArray::createWithCapacity(objectGroup->getObjects()->count()); //����object���������������mapObjects����

	CCDictionary *object = NULL;
	CCObject *obj = NULL;
	CCString* type;
	CCPoint position, coord, origin;
	//����������������壬���������Type����Ϊ"TrashCan"�ģ�����Ͱ��,����λ�õ�����
	CCARRAY_FOREACH(objectGroup->getObjects(), obj)
	{
		object = (CCDictionary *)obj;
		type = CCString::create(object->valueForKey("Type")->getCString());
		if (type && type->compare("TrashCan")==0)
		{
			//�������������������ת��������Ƭ��ͼ�е�����ת��Ϊcocos2dx�е�����
			position = ccp(object->valueForKey("x")->floatValue(), object->valueForKey("y")->floatValue());
			coord = this->tileCoordForPosition(position);
			//��ȡ�������ԭ��λ��
			origin = this->tilePositionForCoord(coord, ccp(0.f, 0.f));

			//��������Ͱ������������
			TrashCan *trashCan = TrashCan::create();
			trashCan->setScale(trashCan->getScale()*kScaleFactor);
			CCSize scaleSize = CCSizeMake(trashCan->getContentSize().width*kScaleFactor, trashCan->getContentSize().height*kScaleFactor);
			CCPoint actualOrigin = ccpMult(origin, kPointFactor);
			trashCan->setPosition(ccp(actualOrigin.x+scaleSize.width*trashCan->getAnchorPoint().x,
				actualOrigin.y+scaleSize.height*trashCan->getAnchorPoint().y));

			_actors->addChild(trashCan);
			_mapObjects->addObject(trashCan);
		}

	}
	_mapObjects->retain();
	//column = event->valueForKey("Column")->intValue();
}

cocos2d::CCPoint GameLayer::tileCoordForPosition(cocos2d::CCPoint position)
{
	//����������Ƭ��ͼ�е�����ת���ɵ�ͼ������꣨�������������꣩
	float tileWidth = _tileMap->getTileSize().width;
	float tileHeight = _tileMap->getTileSize().height;
	float levelHeight = _tileMap->getMapSize().height * tileHeight;//��ͼ�߶�

	float x = floor(position.x / tileWidth);
	float y = floor((levelHeight - position.y)/tileHeight);
	return ccp(x, y);
}

CCPoint GameLayer::tilePositionForCoord(cocos2d::CCPoint coord, CCPoint anchorPoint)
{
	//��������ϵ�Ĳ�ͬ��������Ҫ����Ƭ��ͼ�е�����ֵ��ԭ�������Ͻǣ�ת��Ϊ��cocos2dx�е�����ֵ��Զ�������½ǣ�
	float w = _tileMap->getTileSize().width;
	float h = _tileMap->getTileSize().height;
	CCPoint pos = ccp((coord.x*w)+(w*anchorPoint.x),
		(_tileMap->getMapSize().height-coord.y-1)*h + h*anchorPoint.y);
	return pos;
}

void GameLayer::objectCollisionsForSprite(ActionSprite *sprite)
{
	MapObject *mapObject = NULL;
	CCObject *obj = NULL;
	CCRect spriteRect;
	CCRect mapRect;
	CCARRAY_FOREACH(_mapObjects, obj)
	{
		mapObject = (MapObject *)obj;
		spriteRect = sprite->feetCollisionRect(); //��ȡ����Ų�����
		mapRect = mapObject->collisionRect();
		//����Ƿ�����������ײ
		if (spriteRect.intersectsRect(mapRect))
		{
			//CCLog("collide occur!");
			float x = sprite->getDesiredPosition().x;
			float y = sprite->getDesiredPosition().y;

			CCRect intersection = Tools::intersectsRect(spriteRect, mapRect); //��ȡ��������ӵ��ཻ����

			CCLog("HeroRect ori.x :%f, ori.y :%f, w :%f, h :%f", spriteRect.origin.x, spriteRect.origin.y, spriteRect.size.width,spriteRect.size.height);
			CCLog("InterRect ori.x :%f, ori.y :%f, w :%f, h :%f", intersection.origin.x, intersection.origin.y, intersection.size.width,intersection.size.height);
			if (intersection.size.width > intersection.size.height) //����ཻ����Ŀ�ȱȸ߶ȴ�˵������Ǵ��ϻ��½Ӵ����ӵ�
			{
				if (sprite->_groundPosition.y < mapObject->getPositionY()) //��Ҵ������ϽӴ�����
				{
					y = sprite->getDesiredPosition().y - intersection.size.height; //�����y�����Ҫ��������һ�㣬λ��������ײ����±߽磬ʹ֮��Ҫ������������
				}
				else  //����Ǵ������½Ӵ����ӵ�
				{
					y = sprite->getDesiredPosition().y + intersection.size.height;
				}
			}
			else //�ཻ����ĸ߶ȱȿ�ȴ�˵������Ǵ�����ҿ������ӵ�
			{
				if (sprite->_groundPosition.x < mapObject->getPositionX()) //�����Ҵ���ӽ�����
				{
					x = sprite->getDesiredPosition().x - intersection.size.width; //�����x����������������Ҫ������������ڲ�
				}
				else
				{
					x = sprite->getDesiredPosition().x + intersection.size.width;
				}
			}

			sprite->setDesiredPosition(ccp(x, y ));  //���þ����λ��
		}
	}
}

void GameLayer::exitLevel()
{
	_eventState = kEventStateScripted;
	float exitX = _tileMap->getTileSize().width * _tileMap->getMapSize().width * kPointFactor + _hero->getCenterToSides();
	_hero->enterFrom(_hero->getPosition(), ccp(exitX, _hero->getPositionY()));
}

void GameLayer::setEventState(EventState eventState)
{
	_eventState = eventState;

	if (_eventState == kEventStateFreeWalk)
	{
		_hud->showGoMessage();
	}
}

DamageNumber* GameLayer::getDamageNumber()
{
	DamageNumber* number = NULL;
	CCObject* obj = NULL;
	CCARRAY_FOREACH(_damageNumbers, obj)
	{
		number = (DamageNumber *)obj;
		if(number) //�����number��û�ж�������ʾδ�ù����ͷ�����
		{
			if(number->numberOfRunningActions() == 0)
				return number;
		}
	}
	return number;
}

HitEffect* GameLayer::getHitEffect()
{
	HitEffect *effect = NULL;
	CCObject *obj = NULL;
	CCARRAY_FOREACH(_hitEffects, obj)
	{
		effect = (HitEffect *)obj;
		if (effect)
		{
			if (effect->numberOfRunningActions()==0)
			{
				return effect;
			}
		}
	}
	return effect; //�Ҳ������ؿ�
}

Weapon* GameLayer::getWeapon()
{
	Weapon *weapon = NULL;
	CCObject *obj  = NULL;
	CCARRAY_FOREACH(_weapons, obj)
	{
		weapon = (Weapon *)obj;
		if (weapon->_weaponState == kWeaponStateNone)
		{
			return weapon;
		}
	}

	return NULL;
}