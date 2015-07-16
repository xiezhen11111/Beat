#ifndef _Defines_
#define _Defines_

#define SCREEN CCDirector::sharedDirector()->getWinSize()

#define CENTER ccp(SCREEN.width/2, SCREEN.height/2)

#define OFFSCREEN ccp(-SCREEN.width, -SCREEN.height)

//���ص�ǰ�豸��ʱ��
#define CURTIME GetCurTime()

#define IS_IPHONE5() (CCDirector::sharedDirector()->getWinSize().width)

#define max(a,b)  (((a) > (b)) ? (a) : (b))
#define min(a,b)  (((a) < (b)) ? (a) : (b))

#define kScaleFactor 1.0

#define kPointFactor 1.0

#define kTagButtonA 1
#define kTagButtonB 2

#define kOneAtkTime 0.14f

#define DRAW_DEBUG_SHAPES 0   //��Ϊ1��������ײ���򣬵�Ч�ʺܵ���

#define kGravity 1000.0 * kPointFactor
#define kJumpForce 340.0 * kPointFactor
#define kJumpCutoff 130.0 * kPointFactor
#define kPlaneHeight 10.0 * kPointFactor

//������ 0-1 ֮��������
static float realRand_0_1()
{
	cocos2d::cc_timeval psv;
	cocos2d::CCTime::gettimeofdayCocos2d(&psv, NULL);
	//����ʱ������������
	unsigned int tsrans = psv.tv_sec * 1000 + psv.tv_usec / 1000;
	srand(tsrans); //��rand()�����趨����
	return CCRANDOM_0_1();
}

//�����������

//�ڵͺ͸�ֵ��Χ�ڴ���һ���������
#define random_range(low,high) ((rand()%(high-low+1))+low)
//����һ���������ֵ
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
	kActionStateJumpRise,//�뿪�˵��棬��������״̬
	kActionStateJumpFall,//�������
	kActionStateJumpLand,//�䵽����
	kActionStateJumpAttack,
	kActionStateHurt,
	kActionStateKnockedOut,
	kActionStateRecover,
	kActionStateDead,
	kActionStateAutomated,
}ActionState;

/****
 ����Ӵ���ײ�� 
****/
typedef struct _ContactPoint
{
	cocos2d::CCPoint position; //Բ����gamelayer�е�λ��
	cocos2d::CCPoint offset;   //Բ������ھ���ê���ƫ�ƾ���(x, y�ľ���)
	float radius;			   //��ײԲ�İ뾶
}ContactPoint;

/************************************************************************/
/* ��ɫ��                                                               */
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
/* �����˵�AI״̬
***************************/
typedef enum _AIDecision
{
	kDecisionNo = -1,     //û��������
	kDecisionAttack = 0,  //����
	kDecisionStayPut,	  //����
	kDecisionChase,		  //׷��
	kDecisionMove		  //�ƶ�
} AIDecision;

/***************************
/* �¼�����
***************************/
typedef enum _EventState
{
	kEventStateScripted = 0,  //��Ҳ��ܿ��ƽ�ɫ
	kEventStateFreeWalk,	  //��ҿ�����������
	kEventStateBattle,		  //һЩ�ض����������˻����������
	kEventStateEnd
}EventState;

/***************************
/* ��������
***************************/
typedef enum _EnemyType
{
	kEnemyRobot = 0,		  //�ӱ�
	kEnemyBoss				  //BOSS
} EnemyType;

/************************************************************************/
/* Boss ����                                                            */
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

#define  EFFECT_HIT0_WAV "Sounds/hit0.wav"  //��Ч����
#define  EFFECT_HIT1_WAV "Sounds/hit1.wav"
#endif


 