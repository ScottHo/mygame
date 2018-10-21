#include "HolderNode.h"

HolderNode* HolderNode::createHolder(const std::string& filename)
{
    auto sprite = new (std::nothrow) HolderNode;
    if (sprite && sprite->initWithFile(filename))
    {
        sprite->autorelease();
        return sprite;
   }
   CC_SAFE_DELETE(sprite);
   return nullptr;
}