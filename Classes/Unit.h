#ifndef __UNIT_SCENE_H__
#define __UNIT_SCENE_H__
#include "cocos2d.h"
USING_NS_CC;

class Unit : public cocos2d::Sprite
{
public:
	Unit();
	double power() { return dPower; }
	void setPower(double power) { dPower = power; }
	double health() { return dHealth; }
	void setHealth(double health) { dHealth = health; }
private:
	double dHealth;
	double dPower;
};

#endif // __UNIT_SCENE_H__
