#ifndef __LETTERNODE_H__
#define __LETTERNODE_H__
#include "cocos2d.h"
#include <iostream>
#include <string>
USING_NS_CC;

class LetterNode : public cocos2d::Sprite
{
public:
    static LetterNode* createLetter(const std::string& filename);
    char value() const { return cValue; }
    void setValue(char l){ cValue = l; }
    int points() const { return iPoints; }
    void setPoints(int p) { iPoints = p; }
    bool inField(){ return bInField; }
    void setInField(bool f) { bInField = f; }
    int holderTag() { return iHolderTag; }
    void setHolderTag(int ht) { iHolderTag = ht; }

    void scheduleAction(Action* action);
	void update(float delta) override;

private:
	char cValue;
	int iPoints = 1;
	bool bInField = false;
	int iHolderTag = -1;
	std::queue<Action*> qActions;
};
#endif // __LETTERNODE_H__

