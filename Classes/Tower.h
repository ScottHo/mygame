#ifndef __TOWER_H__
#define __TOWER_H__
#include "cocos2d.h"
#include <string>
USING_NS_CC;

class Tower : public cocos2d::Ref
{
public:
	Tower(int d, int as, int um, bool e): 
		iDamage(d), iAttackSpeed(as), iUpgradeModifer(um), bEnabled(e){}
	int damage() const { return iDamage; }
	void setDamage(int d){ iDamage = d; }
	int enabled() const { return bEnabled; }
	void setEnabled(int e){ bEnabled = e; }
	int attackSpeed() const { return iAttackSpeed; }
	void setAttackSpeed(int as) { iAttackSpeed = as; }
	int upgradeModifier() const { return iUpgradeModifer; }
	void setUpgradeModifier(int um){ iUpgradeModifer = um; }

	static int getDamageFromNode(Node* node);
	static void setDamageFromNode(Node* node, int d);
	static bool getEnabledFromNode(Node* node);
	static void setEnabledFromNode(Node* node, bool e);
	static int getUpgradeModifierFromNode(Node* node);
	static void setUpgradeModifierFromNode(Node* node, int um);
	static int getAttackSpeedFromNode(Node* node);
	static void setAttackSpeedFromNode(Node* node, int as);
	static void clearFromNode(Node* node);
private:
	int iDamage;
	int iAttackSpeed;
	int iUpgradeModifer;
	bool bEnabled;
};

#endif // __TOWER_H__
