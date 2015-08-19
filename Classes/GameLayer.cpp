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

		//由于人物的素材使用来自了同一个纹理集所以这里使用了CCSpriteBatchNode，这样可以只渲染一次，提高了渲染了效率（注意在3.0beta之后推出了自动批次处理所以不在需要使用CCSpriteBatchNode）
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

	_actors->addChild(_hero->getShadow(), 1);         //添加影子
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

		robot->setPosition(OFFSCREEN);//当创建后，被设置到深处，看不到，直到被生产
		robot->_groundPosition = robot->getPosition();
		robot->setDesiredPosition(robot->getPosition());
		robot->setVisible(false);//还需要设置为不可见，这样引擎就不需要去渲染它了
		robot->setColorSet(kColorRandom);
		robot->_actionState = kActionStateNone;

		/*
		设置了所有的机器人远离是因为你不想这50个机器人总是追逐英雄，代替的是当你需要的时候会生产，根据当前的battle event
		*/


		/*
		//机器人出现的范围，从屏幕中间到场景最右边
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
	//打击数字
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

	//打击效果
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
	if (_eventState != kEventStateScripted) //在脚本控制期内不得操纵英雄
	{
		//CCLog("Change dir");
		//设定一下判定框

		CCPoint directionVector = this->vectorForDirection(direction); //将方向改成向量
		_hero->flipSpriteForVelocity(directionVector); //角色设定转向
		//要是双击方向时间满足且方向没改变且没有武器且方向是正右或正左(这个条件其实可以去掉),执行跑步动作（只所以需要判断是否持有武器是因为武器没有相应跑的动画）
		if (!_hero->_weapon && _runDelay >0 && _previousDirection == direction && 
			(direction == kActionDPadDirectionRight || direction == kActionDPadDirectionLeft))
		{
			_hero->runWithDirection(directionVector);
		}
		else if (_hero->_actionState == kActionStateRun &&  //假如英雄跑动中方向改变了，变为走路
			abs(_previousDirection - direction)<=1)
		{
			_hero->moveWithDirection(directionVector);
		}
		else //没到跑的条件（双击间隔时间不够），还是走
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
		if (actionButton->getTag() == kTagButtonA) //按了攻击键
		{
			bool pickedUpWeapon = false; //是否拿起武器
			if (!_hero->_weapon)
			{
				Weapon *weapon = NULL;
				CCObject *objWea = NULL;
				CCARRAY_FOREACH(_weapons, objWea)
				{
					weapon = (Weapon *)objWea;
					if (weapon->_weaponState == kWeaponStateUnequipped)//武器要是卸下的
					{
						if (this->collisionBetweenPlayer(_hero, weapon))//碰撞到了
						{
							pickedUpWeapon = _hero->pickUpWeapon(weapon);//拾取
							_actors->reorderChild(weapon, _hero->getZOrder()+1);
							//int orderW = weapon->getZOrder();
							//int orderH = _hero->getZOrder();
							break;
						}
					}
				}
			}

			//没有拾取武器，则攻击
			if(!pickedUpWeapon)
				_hero->attack();
		}
		else if (actionButton->getTag() == kTagButtonB) //按下了跳跃键
		{
			if (_hero->_weapon)
			{
				//卸下武器
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
		if (actionButton->getTag()==kTagButtonB) //松开的是跳跃键
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

	//更新机器人的位置
	Robot *robot = NULL;
	CCObject* objRob = NULL;
	CCARRAY_FOREACH(_robots, objRob)
	{
		robot = (Robot *)objRob;
		robot->update(dt);
	}

	this->updatePositions();

	//更新人工智能
	ArtificialIntelligence *brain = NULL;
	CCObject *obj = NULL;
	CCARRAY_FOREACH(_brains, obj)
	{
		brain = (ArtificialIntelligence *)obj;
		brain->update(dt);
	}

	this->reorderActors();
	//跑步间隔
	if (_runDelay > 0)
	{	_runDelay -= dt;
	}
	//this->setViewpointCenter(_hero->getPosition());
	//只有当自由行走时，摄像机才跟随玩家移动，战斗发生时不会跟随玩家移动
	if(_eventState == kEventStateFreeWalk || _eventState == kEventStateScripted)
		this->setViewpointCenter(_hero->getPosition());

	this->updateEvent();

	//不断的更改视野偏移，使得视角平滑的移动
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
	float mapWidth = _tileMap->getMapSize().width * _tileMap->getTileSize().width * kPointFactor; //获取地图实际大小
	float floorHeight = 3 * _tileMap->getTileSize().height * kPointFactor; //地图上的地板(玩家可走的范围)实际就是最下面三个格 ，很蹉的办法

	float posX, posY;

	if (_hero->_actionState > kActionStateNone)
	{
		this->objectCollisionsForSprite(_hero); //处理英雄与箱子的碰撞

		//如果是自由行走状态，玩家行动范围跟以前一样
		if (_eventState == kEventStateFreeWalk)
		{
			//此处判定相当复杂，建议新手多写几个判断语句，大可不必弄的这么晦涩难懂
			//作者的意思是，posX的位置当然是要取hero的position(_hero->getDesiredPosition().x),但又不能超过最右边界：mapWidth - _hero->feetCollisionRect().size.width/2
			//同时角色位置最小又不能小于左边界:_hero->feetCollisionRect().size.width/2, 然后就用MIN和MAX加以巧妙的组合实现了一条语句完成多种条件判定的方法
			/*
			其实也很简单，角色位置最小不能小于左边界那么就使用MAX并且其中一个参数是左边界，这样无论另一个值是多少，MAX的结果都不会小于左边界
			同时不能超过最右边界就使用MIN并且其中一个参数是右边界，这样无论另一个值多少，MIN的值都不会大于右边界
			将二者结合就可以保证posX的值不小于左边界且不大于右边界
			*/
			posX = MIN(mapWidth - _hero->feetCollisionRect().size.width/2, 
				MAX(_hero->feetCollisionRect().size.width/2, _hero->getDesiredPosition().x));
			posY = MIN(floorHeight + (_hero->getCenterToBottom() - _hero->feetCollisionRect().size.height), 
				MAX(_hero->getCenterToBottom(), _hero->getDesiredPosition().y));

			_hero->setGroundPosition(ccp(posX, posY));
			_hero->setPosition(ccp(_hero->_groundPosition.x, _hero->_groundPosition.y + _hero->getJumpHeight()));
		}
		// 开战状态时，玩家行动范围被固定在eventCenter范围之内，即只在战事发生范围（以eventCenter为中心，左右半个屏幕范围）可动
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

	//设置机器人的位置
	Robot* robot = NULL;
	CCObject* obj = NULL;
	CCARRAY_FOREACH(_robots, obj)
	{
		robot = (Robot *)obj;
		if (robot->_actionState > kActionStateNone)
		{
			this->objectCollisionsForSprite(robot); //处理机器人与箱子的碰撞
			//第一，你不限制机器人的x轴坐标。机器人被允许在屏幕以外，这是合乎逻辑的，因为它们出现在地图上
			//其次，当一个死机器人与英雄的距离大于屏幕的可视区域，然后机器人变得不可见and复位
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

	//更新武器位置
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
	this->setPosition(ccp(viewPoint.x + _viewPointOffset, viewPoint.y));//为了平滑的过渡到英雄为中心的视角，而不是突然移动
}

void GameLayer::reorderActors()
{
	int spriteZ = 0;

	//先更新地上武器的Zorder，防止后面更新英雄手部武器时zorder发生冲突
	CCObject *obj2 = NULL;
	Weapon *objWeapon = NULL;
	CCARRAY_FOREACH(_weapons, obj2)
	{
		objWeapon = (Weapon *)obj2;
		spriteZ = this->getZFromYPosition(objWeapon->_shadow->getPositionY());
		_actors->reorderChild(objWeapon->_shadow, spriteZ);
		_actors->reorderChild(objWeapon, spriteZ);
	}

	spriteZ = this->getZFromYPosition(_hero->getShadow()->getPositionY()); //hero的zorder
	//更新英雄的zorder
	_actors->reorderChild(_hero->getShadow(), spriteZ);
	_actors->reorderChild(_hero, spriteZ);

	if(_hero->_weapon) //有武器时刻让它处于上层，否则移动底部时会出现错误
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
	//y坐标值越小，返回的z值越大显示也就越靠前并且保证了z值都是正值
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
		
		//绘制侦察圆
		ccDrawColor4B(0, 0, 255, 255);
		ccDrawCircle(sprite->getPosition(), sprite->_detectionRadius, 0, 16, false, 1.0f, 1.0f);

		ccDrawColor4B(0, 255, 0, 255);

		//绘制接触圆
		for (i=0; i<sprite->_contactPointCount; i++)
		{
			ccDrawCircle(sprite->_contactPoints[i].position, sprite->_contactPoints[i].radius, 0, 8, false);
		}

		//攻击判定圆
		ccDrawColor4B(255, 0,0, 255);
		for (i=0; i<sprite->_attackPointCount; i++)
		{
			ccDrawCircle(sprite->_attackPoints[i].position, sprite->_attackPoints[i].radius, 0, 8, false);
		}

		//绘制脚部矩形框
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
		//敌人挂掉了，计数减1
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
	bool didHit = false;  //有没有打到别人
	CCPoint attackPosition; //这个Point是用来存储碰撞点的
	if (actionSprite == _hero) //如果是英雄，检测与每个机器人的攻击碰撞
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
					bool bShowEffect = true; //是否有打击特效
					
					//攻击到敌人，飘出伤害字
					DamageNumber* damageNumber = this->getDamageNumber();

					//注意hurtWithDamage是重载函数，可以设置受击的敌人是会后退还是被打到在地

					if (_hero->_actionState == kActionStateJumpAttack) //跳跃攻击
					{
						SimpleAudioEngine::sharedEngine()->playEffect(EFFECT_HIT1_WAV); //播放打击音效

						robot->knockoutWithDamage(_hero->getJumpAttackDamage(), ccp(_hero->_directionX, 0));
						damageNumber->showWithValue(_hero->getJumpAttackDamage(), COLOR_MIDHP,robot->getPosition());
						bShowEffect = false;
					}
					else if(_hero->_actionState == kActionStateRunAttack) //跑步攻击
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
						//与第一次受击表现相同
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

					didHit = true; //表示打到机器人了

					if (bShowEffect) //播放打击特效
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

		//判断与物体的碰撞
		MapObject *mapObject = NULL;
		CCObject *object = NULL;
		CCARRAY_FOREACH(_mapObjects, object)
		{
			mapObject = (MapObject *)object;
			if (this->collisionBetweenAttacker(_hero, mapObject, &attackPosition)) //如果攻击到了箱子
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
				_hud->setHitPoints(_hero->getHitPoints(), _hero->getMaxHitPoints());//设置血量
				didHit = true;

				DamageNumber *damageNumber = this->getDamageNumber();
				damageNumber->showWithValue(_boss->getAttackDamage(),COLOR_MIDHP, _hero->getPosition());

				HitEffect *hitEffect = this->getHitEffect();
				_actors->reorderChild(hitEffect, MAX(_boss->getZOrder(), _hero->getZOrder()+1));
				hitEffect->showEffectAtPosition(attackPosition);
			}
		}
	}
	else   //如果是机器人，检测是否打到了英雄
	{
		if(_hero->_actionState < kActionStateKnockedOut && 
			_hero->_actionState != kActionStateNone && _hero->_isInvincible==false)
		{
			CCPoint attackPosition;
			if (this->collisionBetweenAttacker(actionSprite, _hero, &attackPosition))
			{
				SimpleAudioEngine::sharedEngine()->playEffect(EFFECT_HIT0_WAV);
				_hero->hurtWithDamage(actionSprite->getAttackDamage(), actionSprite->getAttackForce(), ccp(actionSprite->_directionX, 0.0f));
				_hud->setHitPoints(_hero->getHitPoints(), _hero->getMaxHitPoints()); //设置血量
				didHit = true; //打到了英雄

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

//与敌人的碰撞检测
bool GameLayer::collisionBetweenAttacker(ActionSprite* attacker, ActionSprite* target, cocos2d::CCPoint* position)
{
	//first; 检查是否在同一平面上 即角色脚y坐标是否接近
	float planeDist = attacker->getShadow()->getPositionY() - target->getShadow()->getPositionY();

	
	if (fabsf(planeDist) <= kPlaneHeight) //判断y坐标相差是否在一个平面允许的误差内
	{
		int i,j;
		//探测边界（两个物体的侦测半径之和）
		float combinedRadius = attacker->_detectionRadius + target->_detectionRadius;

		//initial detection 判断攻击者与被攻击者是否进入了探测范围（不开方效率高）
		if (ccpDistanceSQ(attacker->getPosition(), target->getPosition()) <= combinedRadius * combinedRadius)
		{
			//相交了，进行下一步检测

			int attackPointCount = attacker->_attackPointCount;
			int contactPointCount = target->_contactPointCount;

			ContactPoint attackPoint, contactPoint;
			//secondary detection
			//检查攻击者所有的攻击点与被攻击者的碰撞点是否有接触
			for (i=0; i<attackPointCount; i++)
			{
				attackPoint = attacker->_attackPoints[i];
				for (j=0; j<contactPointCount; j++)
				{
					contactPoint = target->_contactPoints[j];
					combinedRadius = attackPoint.radius + contactPoint.radius;
					//如果在碰撞范围
					if (ccpDistanceSQ(attackPoint.position, contactPoint.position) <= combinedRadius * combinedRadius)
					{
						//返回碰撞点位置
						position->x = attackPoint.position.x;
						position->y = attackPoint.position.y;
						return true;
					}
				}
			}
		}
	}

	//为什么要圆形而不使用矩形类检测碰撞，是因为矩形检测比较麻烦要关注很多点（156页），而矩形只需要关注两个物体的坐标
	return false;
}

bool GameLayer::collisionBetweenAttacker(ActionSprite* attacker, MapObject* object, cocos2d::CCPoint* position)
{
	//1 检查是否在同一平面
	float objectBottom = object->collisionRect().origin.y;  //物体的底部高度
	float objectTop = objectBottom + object->collisionRect().size.height; //物体的顶部高度
	float attackerBottom = attacker->feetCollisionRect().origin.y;  //攻击者的足部高度
	float attackerTop = attackerBottom + attacker->feetCollisionRect().size.height;  //攻击者的足部顶所处的高度
	//判断攻击者的足部矩形范围是否在object的范围之内
	if ((attackerBottom>objectBottom && attackerBottom<objectTop) || (attackerTop>objectBottom && attackerTop<objectTop))
	{
		int i, j;
		float combinedRadius = attacker->_detectionRadius + object->_detectionRadius;

		//判断攻击者与物体距离是否在两者的侦测范围之内
		if (ccpDistanceSQ(attacker->getPosition(), object->getPosition()) <= combinedRadius*combinedRadius)
		{
			int attackPointCount = attacker->_attackPointCount;
			int contactPointCount = object->_contactPointCount;

			ContactPoint attackPoint, contactPoint;
			//针对每个攻击点，对箱子的每个接触判定点进行碰撞检测判断
			for (i=0; i<attackPointCount; i++)
			{
				attackPoint = attacker->_attackPoints[i];
				for (j=0; j<contactPointCount; j++)
				{
					contactPoint = object->_contactPoints[j];
					combinedRadius = attackPoint.radius + contactPoint.radius;
					//检测攻击点与接触点是否相撞,如果距离够近，返回true
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

		//探测武器与玩家的距离是否在侦测范围内
		if (ccpDistanceSQ(player->getPosition(), weapon->getPosition()) <= combineRadius * combineRadius)
		{
			int contactPointCount = player->_contactPointCount;
			ContactPoint contactPoint;

			//判断玩家身上每一个接触判定框是否在武器的触摸范围内
			for (i=0; i<contactPointCount; i++)
			{
				contactPoint = player->_contactPoints[i];
				combineRadius = contactPoint.radius + weapon->_detectionRadius;
				if (ccpDistanceSQ(contactPoint.position, weapon->getPosition()) <= combineRadius * combineRadius)
				{
					return true;  //有一个接触了，就返回true
				}
			}
		}
	}
	return false;
}

void GameLayer::loadLevel(int level)
{
	//获取Levels.plist的完整路径
	std::string levelsPlist = CCFileUtils::sharedFileUtils()->fullPathForFilename("Levels.plist");
	//把Levels.plist的内容转换为数组
	CCArray* levelArray = CCArray::createWithContentsOfFile(levelsPlist.c_str());
	//读取数组中的第一个数组，就是第一关数组
	CCDictionary *levelData = CCDictionary::createWithDictionary((CCDictionary *)(levelArray->objectAtIndex(level)));

	CCString *tileMap = (CCString *)levelData->objectForKey("TileMap");
	this->initTileMap(tileMap->getCString());

	_battleEvents = CCArray::createWithArray((CCArray *)levelData->objectForKey("BattleEvents"));
	_battleEvents->retain();
	
	_totalLevel = levelArray->count();
	_currentLevel = level;

	//处理boss
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

	//轮循小兵数组
	CCObject *obj  = NULL;
	CCARRAY_FOREACH(enemies, obj)
	{
		enemyData = (CCDictionary *)obj;
		//获取行，列，偏移数据，确定敌人的类型与位置
		row = enemyData->valueForKey("Row")->floatValue();
		type = enemyData->valueForKey("Type")->intValue();
		offset = enemyData->valueForKey("Offset")->floatValue();

		//如果类型是小兵，则(还有BOSS类型，但目前没做)
		if (type == kEnemyRobot)
		{
			color = enemyData->valueForKey("Color")->intValue();

			//获取一个未用过的robot（该数组在initRobots方法中已经进行处理）
			CCARRAY_FOREACH(_robots, obj)
			{
				robot = (Robot *)obj;
				//初始化时robot的状态为None
				if (robot->_actionState == kActionStateNone)
				{
					robot->stopAllActions();
					robot->setVisible(false);
					//根据机器人的参数计算出位置
					CCPoint pos = ccp(
						//CENTER.x + robot->getCenterToSides()的位置正好是在两边的边界位置，如果offset为1则在最右边，-1则在最左边
						origin+(offset * (CENTER.x + robot->getCenterToSides())), 
						//从机器人的脚底向上加上所在行的高度
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
		//当前敌人消灭

		//设置视野
		float maxCenterX = _tileMap->getMapSize().width * _tileMap->getTileSize().width * kPointFactor - CENTER.x;
		float cameraX = MAX(MIN(_hero->getPosition().x, maxCenterX), CENTER.x);
		_viewPointOffset = cameraX - _eventCenter;

		//如果本关所有的战斗事件都完了，就退出本关
		if(_battleEvents->count()==0)
			this->exitLevel();
		else
			this->setEventState(kEventStateFreeWalk);//切换回去
	}
	else if (_eventState == kEventStateFreeWalk)
	{
		this->cycleEvents();
	}
	else if (_eventState == kEventStateScripted) //add this
	{
		float exitX = _tileMap->getTileSize().width * _tileMap->getMapSize().width * kPointFactor + _hero->getCenterToSides();
		//英雄走出关卡边界，即到下一关
		if (_hero->getPositionX() >= exitX)
		{
			_eventState = kEventStateEnd;
			if (_currentLevel < _totalLevel - 1)
			{
				CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(1.f, GameScene::nodeWithLevel(_currentLevel+1)));
			}
			else //所有关卡打完，结束游戏
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

	//遍历未发生事件的集合，获取每个事件列的位置同时计算_eventCenter位置，判断哪个处于屏幕中心
	CCARRAY_FOREACH(_battleEvents, obj)
	{
		event = (CCDictionary *)obj;
		column = event->valueForKey("Column")->intValue();//玩家走到该列后激活敌人
		float maxCenterX = _tileMap->getMapSize().width * _tileMap->getTileSize().width * kPointFactor - CENTER.x;
		float columnPosition = column * tileWidth - tileWidth/2;
		_eventCenter = MAX(MIN(columnPosition, maxCenterX), CENTER.x);

		if (_hero->getPosition().x >= _eventCenter)  //玩家走过屏幕半场了，激活敌人
		{
			_currentEvent = event;
			_eventState = kEventStateBattle;//设置当前事件状态为战斗
			//_battleEvents = CCArray::createWithArray((CCArray *)levelData->objectForKey("BattleEvents"));
			CCArray *enemyData = CCArray::createWithArray((CCArray *)event->objectForKey("Enemies"));//获取敌人的数据（包含敌人的类型，位置，颜色）
			_activeEnemies = enemyData->count();
			this->spawnEnemies(enemyData, _eventCenter);//创建敌人
			this->setViewpointCenter(ccp(_eventCenter, _hero->getPositionY()));//设置视角
			break;
		}
	}

	//从未发生事件集合中移除掉
	if (_eventState == kEventStateBattle)
	{
		_battleEvents->removeObject(_currentEvent);
	}
}

void GameLayer::onEnterTransitionDidFinish()
{
	CCLayer::onEnterTransitionDidFinish();

	_hud->displayLevel(_currentLevel+1); //显示关卡信息

	//刚进入关卡时角色是自己走进来的，不受玩家控制
	_eventState = kEventStateScripted;
	_hero->enterFrom(_hero->getPosition(), ccp(64.f, _hero->getPositionY()));

	//开启定时器，延时2s执行，执行1次，执行triggerEvent方法切换状态为kEventStateFreeWalk
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
	CCTMXObjectGroup *objectGroup = _tileMap->objectGroupNamed("Objects");    //首先获取地图的object层
	this->_mapObjects = CCArray::createWithCapacity(objectGroup->getObjects()->count()); //根据object层的物体数来生成mapObjects数组

	CCDictionary *object = NULL;
	CCObject *obj = NULL;
	CCString* type;
	CCPoint position, coord, origin;
	//遍历对象层所有物体，如果有物体Type属性为"TrashCan"的（垃圾桶）,设置位置等属性
	CCARRAY_FOREACH(objectGroup->getObjects(), obj)
	{
		object = (CCDictionary *)obj;
		type = CCString::create(object->valueForKey("Type")->getCString());
		if (type && type->compare("TrashCan")==0)
		{
			//下面进行了两个方法的转换，将瓦片地图中的坐标转换为cocos2dx中的坐标
			position = ccp(object->valueForKey("x")->floatValue(), object->valueForKey("y")->floatValue());
			coord = this->tileCoordForPosition(position);
			//获取垃圾箱的原点位置
			origin = this->tilePositionForCoord(coord, ccp(0.f, 0.f));

			//创建垃圾桶，设置其坐标
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
	//将对象处于瓦片地图中的坐标转换成地图块的坐标（所处的行列坐标）
	float tileWidth = _tileMap->getTileSize().width;
	float tileHeight = _tileMap->getTileSize().height;
	float levelHeight = _tileMap->getMapSize().height * tileHeight;//地图高度

	float x = floor(position.x / tileWidth);
	float y = floor((levelHeight - position.y)/tileHeight);
	return ccp(x, y);
}

CCPoint GameLayer::tilePositionForCoord(cocos2d::CCPoint coord, CCPoint anchorPoint)
{
	//由于坐标系的不同，所以需要将瓦片地图中的坐标值（原点在左上角）转换为在cocos2dx中的坐标值（远点在左下角）
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
		spriteRect = sprite->feetCollisionRect(); //获取精灵脚部矩形
		mapRect = mapObject->collisionRect();
		//检测是否与垃圾箱碰撞
		if (spriteRect.intersectsRect(mapRect))
		{
			//CCLog("collide occur!");
			float x = sprite->getDesiredPosition().x;
			float y = sprite->getDesiredPosition().y;

			CCRect intersection = Tools::intersectsRect(spriteRect, mapRect); //获取精灵与盒子的相交矩阵

			CCLog("HeroRect ori.x :%f, ori.y :%f, w :%f, h :%f", spriteRect.origin.x, spriteRect.origin.y, spriteRect.size.width,spriteRect.size.height);
			CCLog("InterRect ori.x :%f, ori.y :%f, w :%f, h :%f", intersection.origin.x, intersection.origin.y, intersection.size.width,intersection.size.height);
			if (intersection.size.width > intersection.size.height) //如果相交矩阵的宽度比高度大，说明玩家是从上或下接触箱子的
			{
				if (sprite->_groundPosition.y < mapObject->getPositionY()) //玩家从下向上接触箱子
				{
					y = sprite->getDesiredPosition().y - intersection.size.height; //精灵的y坐标就要修正向下一点，位于箱子碰撞框的下边界，使之不要进入箱子里面
				}
				else  //玩家是从上向下接触箱子的
				{
					y = sprite->getDesiredPosition().y + intersection.size.height;
				}
			}
			else //相交矩阵的高度比宽度大，说明玩家是从左或右靠近箱子的
			{
				if (sprite->_groundPosition.x < mapObject->getPositionX()) //如果玩家从左接近箱子
				{
					x = sprite->getDesiredPosition().x - intersection.size.width; //把玩家x坐标向左修正，不要让其进入箱子内部
				}
				else
				{
					x = sprite->getDesiredPosition().x + intersection.size.width;
				}
			}

			sprite->setDesiredPosition(ccp(x, y ));  //设置精灵的位置
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
		if(number) //如果此number上没有动作，表示未用过，就返回它
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
	return effect; //找不到返回空
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