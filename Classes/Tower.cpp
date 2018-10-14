#include "Tower.h"

int Tower::getDamageFromNode(Node* node)
{
	Tower* towerPtr = static_cast<Tower*>(node->getUserObject());
	int d = towerPtr->damage();
	return d;
}

void Tower::setDamageFromNode(Node* node, int d)
{
	Tower* towerPtr = static_cast<Tower*>(node->getUserObject());
	towerPtr->setAttackSpeed(d);
}

int Tower::getAttackSpeedFromNode(Node* node)
{
	Tower* towerPtr = static_cast<Tower*>(node->getUserObject());
	int as = towerPtr->attackSpeed();
	return as;
}

void Tower::setAttackSpeedFromNode(Node* node, int as)
{
	Tower* towerPtr = static_cast<Tower*>(node->getUserObject());
	towerPtr->setAttackSpeed(as);
}

int Tower::getUpgradeModifierFromNode(Node* node)
{
	Tower* towerPtr = static_cast<Tower*>(node->getUserObject());
	int um = towerPtr->upgradeModifier();
	return um;
}

void Tower::setUpgradeModifierFromNode(Node* node, int um)
{
	Tower* towerPtr = static_cast<Tower*>(node->getUserObject());
	towerPtr->setUpgradeModifier(um);
}

void Tower::clearFromNode(Node* node)
{
	Tower* towerPtr = static_cast<Tower*>(node->getUserObject());
	delete towerPtr;
}