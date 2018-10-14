#include "Unit.h"

int Unit::getHealthFromNode(Node* node)
{
	Unit* unitPtr = static_cast<Unit*>(node->getUserObject());
	int h = unitPtr->health();
	return h;
}

void Unit::setHealthFromNode(Node* node, int h)
{
	Unit* unitPtr = static_cast<Unit*>(node->getUserObject());
	unitPtr->setHealth(h);
}

void Unit::clearFromNode(Node* node)
{
	Unit* unitPtr = static_cast<Unit*>(node->getUserObject());
	delete unitPtr;
}