#ifndef __TOWER_H__
#define __TOWER_H__
#include "cocos2d.h"
#include <string>
USING_NS_CC;

class Tower : public cocos2d::Ref
{
public:
	Tower(int d, int as, int um): iDamage(d), iAttackSpeed(as), iUpgradeModifer(um){}
	int damage() const { return iDamage; }
	void setDamage(int d){ iDamage = d; }
	int attackSpeed() const { return iAttackSpeed; }
	void setAttackSpeed(int as) { iAttackSpeed = as; }
	int upgradeModifier() const { return iUpgradeModifer; }
	void setUpgradeModifier(int um){ iUpgradeModifer = um; }

	static int getDamageFromNode(Node* node);
	static void setDamageFromNode(Node* node, int d);
	static int getUpgradeModifierFromNode(Node* node);
	static void setUpgradeModifierFromNode(Node* node, int um);
	static int getAttackSpeedFromNode(Node* node);
	static void setAttackSpeedFromNode(Node* node, int as);
	static void clearFromNode(Node* node);
private:
	int iDamage;
	int iAttackSpeed;
	int iUpgradeModifer;
};

#endif // __TOWER_H__
