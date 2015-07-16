#ifndef _HitEffect_
#define _HitEffect_

#include "cocos2d.h"

class HitEffect : public cocos2d::CCSprite
{
public:
	HitEffect();
	~HitEffect();
	CREATE_FUNC(HitEffect);

	bool init();

	CC_SYNTHESIZE_RETAIN(cocos2d::CCAction*, _effectAction, EffectAction);

	void showEffectAtPosition(cocos2d::CCPoint pos);
protected:
private:
};
#endif