/************************************************************************/
/* ���ߺ���
/* Author: ������
/* Date: 2014.7.15
/************************************************************************/
#pragma once
#ifndef _Tools_
#define _Tools_

#include "cocos2d.h"

class Tools
{
public:
	Tools();
	~Tools();

	//�����ཻ����
	static cocos2d::CCRect intersectsRect(const cocos2d::CCRect &rectA,const cocos2d::CCRect &rectB);
protected:
private:
};
#endif