/************************************************************************/
/* 地图上的物体基类
/* 简单版的ActionSprite，不能移动没有任何动作
/************************************************************************/
#pragma once  //此句仅仅是为了去掉警告

#ifndef _MapObject_
#define _MapObject_

#include "cocos2d.h"
#include "Defines.h"

class MapObject : public cocos2d::CCSprite
{
public:
	MapObject();
	~MapObject();
	CREATE_FUNC(MapObject);
	bool init();

	//碰撞的区域，具体由子类具体实现
	virtual cocos2d::CCRect collisionRect();

	void modifyContactPointAtIndex(const int pointIndex, const cocos2d::CCPoint offset, const float radius);
	void modifyPoint(ContactPoint *point, const cocos2d::CCPoint offset, const float radius);
	virtual void destroyed();

	void setPosition(const cocos2d::CCPoint& pos);
	void transformPoints(); //设置接触点

	float _detectionRadius;      //探测半径
	std::vector<ContactPoint> _contactPoints; //碰撞判定点集
	int _contactPointCount;		  //碰撞判定点数
	ObjectState _objectState;	  //物体状态
protected:
private:
};
#endif