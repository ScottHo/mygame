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

void TowerNode::applyLevel()
{
	switch (iLevel)
	{
		case 3:
			setDamage(1);
			setAttackSpeed(1);
			setUpgradeModifier(1);
			break;
		case 4:
			setDamage(2);
			setAttackSpeed(1);
			setUpgradeModifier(1);
			break;
		case 5:
			setDamage(3);
			setAttackSpeed(1);
			setUpgradeModifier(1);
			break;
		case 6:
			setDamage(6);
			setAttackSpeed(1);
			setUpgradeModifier(2);
			break;
		case 7:
			setDamage(7);
			setAttackSpeed(1);
			setUpgradeModifier(2);
			break;
		case 8:
			setDamage(9);
			setAttackSpeed(1);
			setUpgradeModifier(3);
			break;
		case 9:
			setDamage(12);
			setAttackSpeed(1);
			setUpgradeModifier(5);
			break;
		default:
			break;

	}

}

void TowerNode::addTarget(UnitNode* _target)
{
	hasTarget = true;
	targets.push_back(_target);
}

void TowerNode::removeTarget(UnitNode* _target)
{
	std::vector<UnitNode*>::iterator indx = lower_bound(targets.begin(),targets.end(), _target);
	int pos=indx-targets.begin();
	if(indx == targets.end() || *indx!=_target)
	{
	 	std::cout<< "Unit " <<_target->getTag()<<" not found";
	}
	else
	{
		targets.erase(targets.begin()+pos);
		if (targets.size() == 0)
		{
			hasTarget = false;
			setIdle();
		}
	}
}

void TowerNode::clearTargets()
{
	hasTarget = false;
	targets.clear();
	setIdle();
}

void TowerNode::setIdle()
{
	std::stringstream ss;
	ss << "Tower" << iLevel << "Idle.png";
	setTexture(ss.str());
}

void TowerNode::setShooting()
{
	std::stringstream ss;
	ss << "Tower" << iLevel << "Shooting.png";
	setTexture(ss.str());
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
				setShooting();
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
		else if (attackTimer <= .9)
		{
			setIdle();
		}
	}
}