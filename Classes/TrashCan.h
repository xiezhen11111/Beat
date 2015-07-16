/************************************************************************/
/* 装有物品的可破坏箱子类                                               */
/* Author: 杨亦松														*/
/* Date: 2014.7.14														*/
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