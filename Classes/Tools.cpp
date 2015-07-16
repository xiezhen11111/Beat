#include "Tools.h"
#include "Defines.h"
USING_NS_CC;

Tools::Tools()
{

}

Tools::~Tools()
{

}

CCRect Tools::intersectsRect(const CCRect &rectA, const CCRect &rectB)
{
	CCPoint ori;    //左下角顶点     cocos2dx的CCRect是以左下角为原点，w,h为参数(宽和高)，并不是左上角是top,右下角为bottom
	CCPoint topM;	//右下角顶点
	float width = 0.f, height = 0.f;

	if (rectA.intersectsRect(rectB))
	{
		ori.x = max(rectA.getMinX(), rectB.getMinX());
		ori.y = max(rectA.getMinY(), rectB.getMinY());

		topM.x = min(rectA.getMaxX(), rectB.getMaxX());
		topM.y = min(rectA.getMaxY(), rectB.getMaxY());

		width = topM.x - ori.x;
		height = topM.y - ori.y;


		return cocos2d::CCRectMake(ori.x, ori.y, width, height);
	}
	return cocos2d::CCRectMake(0, 0, -1.f, -1.f);
}