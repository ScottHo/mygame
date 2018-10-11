#ifndef __UNIT_SCENE_H__
#define __UNIT_SCENE_H__
#include "cocos2d.h"
USING_NS_CC;

class Unit : public cocos2d::Sprite
{
public:
	Unit();
	double speed() { return dSpeed; }
	void setSpeed(double speed) { dSpeed = speed; }
	double health() { return dHealth; }
	void setHealth(double health) { dHealth = health; }
private:
	double dHealth;
	double dSpeed;
};

#endif // __UNIT_SCENE_H__
