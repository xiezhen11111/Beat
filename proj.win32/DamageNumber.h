#pragma once

#ifndef _DamageNumber_
#define _DamageNumber_

#include "cocos2d.h"
class DamageNumber : public cocos2d::CCLabelBMFont
{
public:
	DamageNumber();
	~DamageNumber();
	CREATE_FUNC(DamageNumber);

	//出现的数值以及位置
	void showWithValue(int value, cocos2d::CCPoint origin);

	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _damageAction, DamageAction);
protected:
	
private:
};
#endif