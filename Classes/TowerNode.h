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
    int level() const { return iLevel; }
    void setLevel(int l){ iLevel = l; }
	void addTarget(UnitNode* _target)
	{
		hasTarget = true;
		targets.push_back(_target);
	}

	void removeTarget(UnitNode* _target)
	{
		std::vector<UnitNode*>::iterator indx = lower_bound(targets.begin(),targets.end(), _target);
		int pos=indx-targets.begin();
		if(indx == targets.end() || *indx!=_target)
		{
		 	std::cout<< "Unit " <<_target->getTag()<<" not found";
		}
		else
		{
			targets.erase(targets.begin()+pos);
			if (targets.size() == 0)
			{
				hasTarget = false;
			}
		}
	}

	void update(float delta) override;

private:
	std::vector<UnitNode*> targets;
	bool hasTarget = false;
	float attackTimer = 1.0;
	int iDamage = 1;
	int iAttackSpeed = 1;
	int iUpgradeModifer = 1;
	int iLevel = 3;
	bool bEnabled = false;
};
#endif // __TOWERNODE_H__

