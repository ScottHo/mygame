#ifndef __HOLDERNODE_H__
#define __HOLDERNODE_H__
#include "cocos2d.h"
#include <iostream>
#include <string>
USING_NS_CC;

class HolderNode : public cocos2d::Sprite
{
public:
    static HolderNode* createHolder(const std::string& filename);
    bool value() const { return bValue; }
    void setValue(bool b){ bValue = b; }
    int letterTag() const { return iLetterTag; }
    void setLetterTag(int lt){ iLetterTag = lt;}
	//void update(float delta) override;

private:
	bool bValue = false;
	int iLetterTag = -1;
};
#endif // __HOLDERNODE_H__

