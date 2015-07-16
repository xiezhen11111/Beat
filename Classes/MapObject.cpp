#include "MapObject.h"
#include "Defines.h"
USING_NS_CC;

MapObject::MapObject()
{
	_detectionRadius = 0.f;
	_contactPointCount = 0;
	//_contactPoints = NULL;
	_objectState = kObjectStateDestroyed;
}

MapObject::~MapObject()
{
	_contactPoints.clear();
}

bool MapObject::init()
{
	return true;
}

void MapObject::destroyed()
{
	this->_objectState = kObjectStateDestroyed;
}

CCRect MapObject::collisionRect()
{
	return CCRectZero;
}

void MapObject::setPosition(const CCPoint& pos)
{
	CCSprite::setPosition(pos);
	this->transformPoints();
}

void MapObject::transformPoints()
{
	for (int i=0; i<_contactPointCount; i++)
	{
		_contactPoints[i].position = ccpAdd(this->getPosition(), ccp(_contactPoints[i].offset.x, _contactPoints[i].offset.y));
	}
}

void MapObject::modifyContactPointAtIndex(const int pointIndex, const cocos2d::CCPoint offset, const float radius)
{
	ContactPoint *contactPoint = &(_contactPoints[pointIndex]);
	this->modifyPoint(contactPoint, offset, radius);
}

void MapObject::modifyPoint(ContactPoint *point, const cocos2d::CCPoint offset, const float radius)
{
	point->offset = ccpMult(offset, kPointFactor);
	point->radius = radius * kPointFactor;
	point->position = ccpAdd(this->getPosition(), point->offset);
}