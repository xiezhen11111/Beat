#pragma once
#ifndef _Boss_
#define _Boss_

#include "cocos2d.h"
#include "ActionSprite.h"

class Boss : public ActionSprite
{
public:
	Boss();
	~Boss();

	CREATE_FUNC(Boss);
	bool init();

	void hurtWithDamage(float damage, float force, cocos2d::CCPoint direction);
	void knockoutWithDamage(float damage, cocos2d::CCPoint direction);
protected:
	void setContactPointsForAction(ActionState actionState);
	void setDisplayFrame(cocos2d::CCSpriteFrame *pNewFrame);
	
private:
};
#endif