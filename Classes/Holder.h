#ifndef __HOLDER_H__
#define __HOLDER_H__
#include "cocos2d.h"
USING_NS_CC;

class Holder : public cocos2d::Ref
{
public:
	Holder(bool b) : bValue(b){}
	bool value() const { return bValue; }
	void setValue(bool b){ bValue = b; }

	static bool getValueFromNode(Node* node);
	static void setValueFromNode(Node* node, bool hasTile);
	static void clearFromNode(Node* node);

private:
	bool bValue;
};

#endif // __HOLDER_H__
