#pragma once

#ifndef _DamageNumber_
#define _DamageNumber_

#include "cocos2d.h"
class DamageNumber : public cocos2d::CCLabelBMFont
{
public:
	DamageNumber();
	~DamageNumber();
	//CREATE_FUNC(DamageNumber);
	bool initWithString(const char *str, const char *fntFile);
	static DamageNumber* create();

	//出现的数值以及位置
	void showWithValue(int value, const cocos2d::ccColor3B& color, cocos2d::CCPoint origin);

	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _damageAction, DamageAction);
protected:
	
private:
};
#endif