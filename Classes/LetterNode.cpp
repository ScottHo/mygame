#include "LetterNode.h"

LetterNode* LetterNode::createLetter(const std::string& filename)
{
    auto sprite = new (std::nothrow) LetterNode;
    if (sprite && sprite->initWithFile(filename))
    {
        sprite->autorelease();
        return sprite;
   }
   CC_SAFE_DELETE(sprite);
   return nullptr;
}

void LetterNode::scheduleAction(Action* action)
{
	//if (getNumberOfRunningActions() == 0)
	//{
		runAction(action);
	//}
	//else
	//{
	//	qActions.push(action);
	//}
}

void LetterNode::update(float dt)
{
	/*if (getNumberOfRunningActions() == 0 and qActions.size() > 0)
	{
		Action* action = qActions.front();
		runAction(action);
		qActions.pop();
	}*/
}