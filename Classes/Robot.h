#ifndef _Robot_
#define _Robot_

#include "cocos2d.h"
#include "ActionSprite.h"

class AnimateGroup;
class Robot : public ActionSprite
{
public:
	Robot();
	~Robot();

	CREATE_FUNC(Robot);
	virtual bool init();

	void setPosition(cocos2d::CCPoint position);
	void setScaleX(float fScaleX);
	void setScaleY(float fScaleY);
	void setScale(float fScale);
	void setVisible(bool bVisible);
	void setColorSet(ColorSet colorSet);

	void setContactPointsForAction(ActionState actionState); //根据当前状态设置碰撞点

	void reset(); //重置操作

	AnimateGroup* animateGroupWithActionWord(const char* actionKeyWord, int frameCount, float delay);
	CC_SYNTHESIZE_RETAIN(cocos2d::CCSprite*, _belt, Belt);
	CC_SYNTHESIZE_RETAIN(cocos2d::CCSprite*, _smoke, Smoke);

	void setDisplayFrame(cocos2d::CCSpriteFrame *pNewFrame);

	void knockoutWithDamage(float damage, cocos2d::CCPoint direction);

	ColorSet _colorSet;

protected:
	
private:
};
#endif