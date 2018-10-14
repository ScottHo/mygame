#ifndef __UNIT_H__
#define __UNIT_H__
#include "cocos2d.h"
#include <string>
USING_NS_CC;

class Unit : public cocos2d::Ref
{
public:
	Unit();
	~Unit();
	Unit(int h): iHealth(h){}
	int health() const { return iHealth; }
	void setHealth(int h){ iHealth = h; }

	static int getHealthFromNode(Node* node);
	static void setHealthFromNode(Node* node, int h);
	static void clearFromNode(Node* node);
private:
	int iHealth;
};

#endif // __UNIT_H__
