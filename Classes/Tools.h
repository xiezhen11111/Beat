/************************************************************************/
/* 工具函数
/* Author: 杨亦松
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

	//求两相交矩阵
	static cocos2d::CCRect intersectsRect(const cocos2d::CCRect &rectA,const cocos2d::CCRect &rectB);
protected:
private:
};
#endif