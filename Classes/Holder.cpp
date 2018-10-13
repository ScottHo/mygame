#include "Holder.h"

bool Holder::getValueFromNode(Node* node)
{
	Holder* holderPtr = static_cast<Holder*>(node->getUserObject());
	bool hasTile = holderPtr->value();
	return hasTile;
}

void Holder::setValueFromNode(Node* node, bool hasTile)
{
	Holder* holderPtr = static_cast<Holder*>(node->getUserObject());
	holderPtr->setValue(hasTile);
}

void Holder::clearFromNode(Node* node)
{
	Holder* holderPtr = static_cast<Holder*>(node->getUserObject());
	delete holderPtr;
}