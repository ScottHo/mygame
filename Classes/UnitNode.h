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
    bool justDied() const { return bJustDied; }
    void setJustDied(bool jd){ bJustDied = jd; }
    bool reachedGoal() { return bReachedGoal; }
    void setReachedGoal()
    { if (iHealth > 0)
    	bReachedGoal = true; 
    }
    void unsetReachedGoal() { bReachedGoal = false; }

	//void update(float delta) override;

private:
	bool bReachedGoal = false;
	int iHealth = 1.0;
	bool bJustDied = false;
};
#endif // __UNITNODE_H__

