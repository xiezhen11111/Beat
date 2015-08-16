#include "DamageNumber.h"
#include "Defines.h"
USING_NS_CC;

DamageNumber::DamageNumber()
{
	_damageAction = NULL;
}

DamageNumber::~DamageNumber()
{

}

DamageNumber* DamageNumber::create()
{	
	DamageNumber *pRet = new DamageNumber();
	if(pRet && pRet->initWithString("0", "Fonts/DamageFont.fnt"))
	{
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return NULL;
}

bool DamageNumber::initWithString(const char *str, const char *fntFile)
{
	CCAssert( (str && fntFile) || (str==NULL && fntFile==NULL), "Invalid params for CCLabelBMFont");

	bool bRet = CCLabelBMFont::initWithString(str, fntFile);
	
	if(bRet == false)
		return bRet;
	//设置一个action：出现，向上漂字，消失
	CCSequence *seq = CCSequence::create(CCShow::create(), CCMoveBy::create(0.6f, ccp(0.f, 40.f*kPointFactor)),
		CCHide::create(),NULL);
	this->setDamageAction(seq);
	return bRet;
}

void DamageNumber::showWithValue(int value, const ccColor3B& color,cocos2d::CCPoint origin)
{
	this->setString(CCString::createWithFormat("%d", value)->getCString());
	this->setColor(color);
	this->setPosition(origin);
	this->stopAllActions();
	this->runAction(_damageAction);
}