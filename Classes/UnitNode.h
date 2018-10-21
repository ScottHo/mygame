#ifndef __UNITNODE_H__
#define __UNITNODE_H__
#include "cocos2d.h"
#include <iostream>
#include <string>
USING_NS_CC;

class UnitNode : public cocos2d::Sprite
{
public:
    static UnitNode* createUnit(const std::string& filename);
    int health() const { return iHealth; }
    void setHealth(int h){ iHealth = h; }

	//void update(float delta) override;

private:
	int iHealth = 1.0;
};
#endif // __UNITNODE_H__

