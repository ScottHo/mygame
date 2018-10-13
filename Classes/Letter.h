#ifndef __LETTER_H__
#define __LETTER_H__
#include "cocos2d.h"
#include <string>
USING_NS_CC;

class Letter : public cocos2d::Ref
{
public:
	Letter(char l, int p) : cValue(l),  iPoints(p){}
	char value() const { return cValue; }
	void setValue(char l){ cValue = l; }
	int points() const { return iPoints; }
	void setPoints(int p) { iPoints = p; }

	static char getLetterFromNode(Node* node);
	static int getPointsFromNode(Node* node);
	static void setPointsFromNode(Node* node, int points);
	static void clearFromNode(Node* node);
private:
	char cValue;
	int iPoints;
};

#endif // __LETTER_H__
