#ifndef __TOWERNODE_H__
#define __TOWERNODE_H__
#include "cocos2d.h"
#include "UnitNode.h"
#include <iostream>
#include <string>
USING_NS_CC;

class TowerNode : public cocos2d::Sprite
{
public:
    static TowerNode* createTower(const std::string& filename);
    int damage() const { return iDamage; }
    void setDamage(int d){ iDamage = d; }
    int enabled() const { return bEnabled; }
    void setEnabled(int e){ bEnabled = e; }
    int attackSpeed() const { return iAttackSpeed; }
    void setAttackSpeed(int as) { iAttackSpeed = as; }
    int upgradeModifier() const { return iUpgradeModifer; }
    void setUpgradeModifier(int um){ iUpgradeModifer = um; }
	void setTarget(UnitNode* _target)
	{
		hasTarget = true;
		target = _target;
	}

	void removeTarget()
	{
		hasTarget = false;
		target = nullptr;
	}

	void update(float delta) override;

private:
	UnitNode* target;
	bool hasTarget = false;
	float attackTimer = 1.0;
	int iDamage = 1;
	int iAttackSpeed = 1;
	int iUpgradeModifer = 1;
	bool bEnabled = false;
};
#endif // __TOWERNODE_H__

