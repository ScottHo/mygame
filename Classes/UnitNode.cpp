#include "UnitNode.h"

UnitNode* UnitNode::createUnit(const std::string& filename)
{
    auto sprite = new (std::nothrow) UnitNode;
    if (sprite && sprite->initWithFile(filename))
    {
        sprite->autorelease();
        return sprite;
   }
   CC_SAFE_DELETE(sprite);
   return nullptr;
}