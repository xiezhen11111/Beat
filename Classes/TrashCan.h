/************************************************************************/
/* װ����Ʒ�Ŀ��ƻ�������                                               */
/************************************************************************/
#pragma once

#ifndef _TrashCan_
#define _TrashCan_

#include "cocos2d.h"
#include "MapObject.h"

class TrashCan : public MapObject
{
public:
	TrashCan();
	~TrashCan();
	CREATE_FUNC(TrashCan);
	bool init();

	void destroyed();
	cocos2d::CCRect collisionRect();

protected:
private:
};
#endif