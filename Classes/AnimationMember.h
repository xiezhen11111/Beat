#ifndef _AnimationMember_
#define _AnimationMember_

#include "cocos2d.h"

class AnimationMember : public cocos2d::CCObject
{
public:
	AnimationMember();
	~AnimationMember();

	static AnimationMember* memberWithAnimation(cocos2d::CCAnimation *animation, cocos2d::CCSprite *target);
	bool initWithAnimation(cocos2d::CCAnimation *animation, cocos2d::CCSprite *target); //�ö����Ͳ��Ŷ�������ʼ��

	void start();   //��ʼ���Ŷ���
	void stop();	//ֹͣ���Ŷ���
	void setFrame(int frameIndex); //���ò��Ŷ����Ķ���(_target)ͼƬΪ�����е�ĳһ֡
protected:
	cocos2d::CCSpriteFrame* _origFrame;  //��ʼ֡
	cocos2d::CCAnimation* _animation;	 //����
	cocos2d::CCSprite *_target;			 //˭�ڲ��Ŷ���
private:
};
#endif