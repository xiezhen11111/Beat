#ifndef _Defines_
#define _Defines_

#define SCREEN CCDirector::sharedDirector()->getWinSize()

#define CENTER ccp(SCREEN.width/2, SCREEN.height/2)

#define OFFSCREEN ccp(-SCREEN.width, -SCREEN.height)

//返回当前设备的时间
#define CURTIME GetCurTime()

#define IS_IPHONE5() (CCDirector::sharedDirector()->getWinSize().width)

#define max(a,b)  (((a) > (b)) ? (a) : (b))
#define min(a,b)  (((a) < (b)) ? (a) : (b))

#define kScaleFactor 1.0

#define kPointFactor 1.0

#define kTagButtonA 1
#define kTagButtonB 2

#define kOneAtkTime 0.14f

#define DRAW_DEBUG_SHAPES 0   //设为1，则绘出碰撞检测框，但效率很低下

#define kGravity 1000.0 * kPointFactor
#define kJumpForce 340.0 * kPointFactor
#define kJumpCutoff 130.0 * kPointFactor
#define kPlaneHeight 10.0 * kPointFactor

//产生真 0-1 之间的随机数
static float realRand_0_1()
{
	cocos2d::cc_timeval psv;
	cocos2d::CCTime::gettimeofdayCocos2d(&psv, NULL);
	//根据时间产生随机种子
	unsigned int tsrans = psv.tv_sec * 1000 + psv.tv_usec / 1000;
	srand(tsrans); //给rand()函数设定种子
	return CCRANDOM_0_1();
}

//随机数生成器

//在低和高值范围内创建一个随机整数
#define random_range(low,high) ((rand()%(high-low+1))+low)
//创建一个随机浮点值
#define frandom ((float)rand()/UINT64_C(0x100000000))
#define frandom_range(low, high) (((high - low) * realRand_0_1()) + low)
#define random_sign (rand() % 2 ? 1 : -1) 

#define COLOR_FULLHP ccc3(95,  255, 106)
#define COLOR_MIDHP ccc3(255, 165, 0)
#define COLOR_LOWHP ccc3(255, 50, 23)

typedef enum _ActionState
{
	kActionStateNone = 0,
	kActionStateIdle,
	kActionStateAttack,
	kActionStateAttackTwo,
	kActionStateAttackThree,
	kActionStateWalk,
	kActionStateRun,
	kActionStateRunAttack,
	kActionStateJumpRise,//离开了地面，处于上升状态
	kActionStateJumpFall,//下落过程
	kActionStateJumpLand,//落到地面
	kActionStateJumpAttack,
	kActionStateHurt,
	kActionStateKnockedOut,
	kActionStateRecover,
	kActionStateDead,
	kActionStateAutomated,
}ActionState;

/****
 身体接触碰撞框 
****/
typedef struct _ContactPoint
{
	cocos2d::CCPoint position; //圆心在gamelayer中的位置
	cocos2d::CCPoint offset;   //圆心相对于精灵锚点的偏移距离(x, y的距离)
	float radius;			   //碰撞圆的半径
}ContactPoint;

/************************************************************************/
/* 颜色集                                                               */
/************************************************************************/
typedef enum _ColorSet
{
	kColorLess = 0,
	kColorCopper,
	kColorSilver,
	kColorGold,
	kColorRandom,
} ColorSet;

/***************************
/* 机器人的AI状态
***************************/
typedef enum _AIDecision
{
	kDecisionNo = -1,     //没作出决定
	kDecisionAttack = 0,  //攻击
	kDecisionStayPut,	  //发呆
	kDecisionChase,		  //追遂
	kDecisionMove		  //移动
} AIDecision;

/***************************
/* 事件类型
***************************/
typedef enum _EventState
{
	kEventStateScripted = 0,  //玩家不能控制角色
	kEventStateFreeWalk,	  //玩家可以自由行走
	kEventStateBattle,		  //一些特定数量机器人会来攻击玩家
	kEventStateEnd
}EventState;

/***************************
/* 敌人类型
***************************/
typedef enum _EnemyType
{
	kEnemyRobot = 0,		  //杂兵
	kEnemyBoss				  //BOSS
} EnemyType;

/************************************************************************/
/* Boss 类型                                                            */
/************************************************************************/
typedef enum _BossType
{
	kBossNone = 0, 
	kBossMohawk
} BossType;

typedef enum _WeaponState
{
	kWeaponStateNone = 0, 
	kWeaponStateEquipped,
	kWeaponStateUnequipped,
	kWeaponStateDestroyed
} WeaponState;

typedef enum _ObjectState
{
	kObjectStateActive,
	kObjectStateDestroyed
}ObjectState;

#define  EFFECT_HIT0_WAV "Sounds/hit0.wav"  //音效常量
#define  EFFECT_HIT1_WAV "Sounds/hit1.wav"
#endif


 