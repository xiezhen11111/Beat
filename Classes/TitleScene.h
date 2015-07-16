#ifndef _TitleScene_
#define _TitleScene_

#include "cocos2d.h"

class TitleScene : public cocos2d::CCScene
{
public:
	TitleScene();
	~TitleScene();

	virtual bool init();
	CREATE_FUNC(TitleScene);
protected:
private:
};
#endif