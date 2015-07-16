/************************************************************************/
/* ��ͼ�ϵ��������
/* Author: ������
/* 2014.7.14
/************************************************************************/
#pragma once  //�˾������Ϊ��ȥ������

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

	virtual cocos2d::CCRect collisionRect();
	void modifyContactPointAtIndex(const int pointIndex, const cocos2d::CCPoint offset, const float radius);
	void modifyPoint(ContactPoint *point, const cocos2d::CCPoint offset, const float radius);
	virtual void destroyed();

	void setPosition(const cocos2d::CCPoint& pos);
	void transformPoints(); //���ýӴ���

	float _detectionRadius;      //̽��뾶
	std::vector<ContactPoint> _contactPoints; //��ײ�ж��㼯
	int _contactPointCount;		  //��ײ�ж�����
	ObjectState _objectState;	  //����״̬
protected:
private:
};
#endif