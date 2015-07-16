#ifndef _AnimateGroup_
#define _AnimateGroup_

#include "cocos2d.h"

/**
*
*����һ��AnimationMembers
�����˵Ķ���������������ɣ�base���̣�����
*�кܴ�Ŀ��������ǻ᲻ͬ��,�����������ڴ���̵ܶ�֡���
*
**/
class AnimateGroup : public cocos2d::CCAnimate
{
public:
	AnimateGroup();
	~AnimateGroup();
	//����������ʼ������ 
	static AnimateGroup* actionWithAnimation(cocos2d::CCAnimation *animation, cocos2d::CCArray *members);
	bool initWithAnimation(cocos2d::CCAnimation *animation, cocos2d::CCArray *members); //�ö�������������ʼ��
	//�ó�Ա������ʼ��
	static AnimateGroup* actionWithAnimation(cocos2d::CCAnimation *animation, int memberCount);
	bool initWithAnimation(cocos2d::CCAnimation *animation, int memberCount); //�ö���������������ʼ��

	void startWithTarget(cocos2d::CCNode *pTarget);
	void stop();  //���ж���ֹͣ

	void update(float dt);

	cocos2d::CCArray* _members;  //������Ա
protected:
	
private:
};
#endif