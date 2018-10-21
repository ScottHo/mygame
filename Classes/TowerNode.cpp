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
			int health = targets[0]->health();
			if (health > 0)
			{
				targets[0]->setHealth(health-iDamage);
				std::cout << "Health down: " << health << "\n";
				if (targets[0]->health() <= 0)
				{
                	targets[0]->setVisible(false);
					targets[0]->setJustDied(true);
					targets.erase(targets.begin());
				}
				else
				{
					auto action = Blink::create(0.25, 2);
					targets[0]->runAction(action);
				}
			}
			attackTimer = 1.0;
			

		}
	}
}