#include "TowerNode.h"

TowerNode* TowerNode::createTower(const std::string& filename)
{
    auto sprite = new (std::nothrow) TowerNode;
    if (sprite && sprite->initWithFile(filename))
    {
        sprite->autorelease();
        return sprite;
   }
   CC_SAFE_DELETE(sprite);
   return nullptr;
}

void TowerNode::update(float delta)
{
	if (hasTarget)
	{
		attackTimer -= delta;
		if (attackTimer <= 0.0)
		{
			int health = target->health();
			if (health > 0)
			{
				target->setHealth(health-iDamage);
				std::cout << "Health down: " << health << "\n";
			}
			attackTimer = 1.0;
		}
	}
}