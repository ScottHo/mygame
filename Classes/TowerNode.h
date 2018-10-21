#ifndef __TOWERNODE_H__
#define __TOWERNODE_H__
#include "cocos2d.h"
#include "Unit.h"
#include <iostream>
#include <string>
USING_NS_CC;

class TowerNode : public cocos2d::Sprite
{
private:
	Node* target;
	bool hasTarget = false;
	float attackTimer = 1.0;
public:
    static TowerNode* createTower(const std::string& filename)
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
	void setTarget(Node* _target)
	{
		hasTarget = true;
		target = _target;
	}

	void removeTarget()
	{
		hasTarget = false;
		target = nullptr;
	}

	void update(float delta) override
	{
		if (hasTarget)
		{
			attackTimer -= delta;
			if (attackTimer <= 0.0)
			{
				int health = Unit::getHealthFromNode(target);
				if (health > 0)
				{
					Unit::setHealthFromNode(target, health-1);
					std::cout << "Health down: " << health << "\n";
				}
				attackTimer = 1.0;
			}
		}
	}

};
#endif // __TOWERNODE_H__

