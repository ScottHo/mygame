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