#include "Letter.h"

char Letter::getLetterFromNode(Node* node)
{
	Letter* letterPtr = static_cast<Letter*>(node->getUserObject());
	char letter = letterPtr->value();
	return letter;
}

int Letter::getPointsFromNode(Node* node)
{
	Letter* letterPtr = static_cast<Letter*>(node->getUserObject());
	int points = letterPtr->points();
	return points;
}

void Letter::setPointsFromNode(Node* node, int points)
{
	Letter* letterPtr = static_cast<Letter*>(node->getUserObject());
	letterPtr->setPoints(points);
}

void Letter::clearFromNode(Node* node)
{
	Letter* letterPtr = static_cast<Letter*>(node->getUserObject());
	delete letterPtr;
}