#include "GameScene.h"
#include "Unit.h"
#include <iostream>

USING_NS_CC;

Scene* Game::createScene()
{
    auto scene = Scene::create();
    auto layer = Game::create();
    scene->addChild(layer);
    
    return scene;
}

bool Game::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    fWindowHeight = Director::getInstance()->getVisibleSize().height;
    fWindowWidth = Director::getInstance()->getVisibleSize().width;

    setupUi();
    setupEvents();

    return true;
}

void Game::setupUi()
{
    // Main frames
    float bottomFrameHeight = fWindowHeight/4.0;
    float mainFrameHeight = fWindowHeight*(3.0/4.0);
    SpriteFrame* bottomFrame = SpriteFrame::create("Grey.png", Rect(0.0, 0.0, fWindowWidth, bottomFrameHeight));
    SpriteFrame* mainFrame = SpriteFrame::create("Blue.png", Rect(0.0, 0.0, fWindowWidth, mainFrameHeight));
	bottomSprite = Sprite::create();
    bottomSprite->setSpriteFrame(bottomFrame);
    bottomSprite->setPosition(0, 0);
    bottomSprite->setAnchorPoint(Vec2(0.0, 0.0));

    mainSprite = Sprite::create();
    mainSprite->setSpriteFrame(mainFrame);
    mainSprite->setPosition(0, bottomFrameHeight);
    mainSprite->setAnchorPoint(Vec2(0.0, 0.0));

    this->addChild(bottomSprite, 0);
    this->addChild(mainSprite, 0);

    SpriteFrame* unitManagerFrame = SpriteFrame::create("Empty.png", Rect(0.0, 0.0, fWindowWidth, mainFrameHeight));
    unitManager = Sprite::create();
    unitManager->setSpriteFrame(unitManagerFrame);
    unitManager->setPosition(0,bottomFrameHeight);
    unitManager->setAnchorPoint(Vec2(0.0,0.0));
    mainSprite->addChild(unitManager, 0, eUnitManager);

    auto button0 = Sprite::create("GreenSquare.png");
    button0->setPosition(fWindowWidth/5.0, bottomFrameHeight/2.0);
    button0->setAnchorPoint(Vec2(0.5, 0.5));
    bottomSprite->addChild(button0, 0, eButton0);

    auto button1 = Sprite::create("GreenSquare.png");
    button1->setPosition(fWindowWidth*(2.0/5.0), bottomFrameHeight/2.0);
    button1->setAnchorPoint(Vec2(0.5, 0.5));
    bottomSprite->addChild(button1, 0, eButton1);

    auto button2 = Sprite::create("GreenSquare.png");
    button2->setPosition(fWindowWidth*(3.0/5.0), bottomFrameHeight/2.0);
    button2->setAnchorPoint(Vec2(0.5, 0.5));
    bottomSprite->addChild(button2, 0, eButton2);

    auto button3 = Sprite::create("GreenSquare.png");
    button3->setPosition(fWindowWidth*(4.0/5.0), bottomFrameHeight/2.0);
    button3->setAnchorPoint(Vec2(0.5, 0.5));
    bottomSprite->addChild(button3, 0, eButton3);

    auto homeBase = Sprite::create("Rectangle.png");
    homeBase->setPosition(0,0);
    homeBase->setAnchorPoint(Vec2(0,0));
    mainSprite->addChild(homeBase, 0, eHomeBase);

    auto enemyBase = Sprite::create("Rectangle.png");
    enemyBase->setPosition(fWindowWidth,0);
    enemyBase->setAnchorPoint(Vec2(1,0));
    mainSprite->addChild(enemyBase, 0, eEnemyBase);
}

void Game::setupEvents()
{
    auto touchListener = EventListenerTouchOneByOne::create();  
    touchListener->onTouchBegan = CC_CALLBACK_2(Game::onTouch, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener,bottomSprite);
}

bool Game::onTouch(Touch* touch, Event* event)
{
    int spriteTouched = touchedSpriteBottom(touch->getLocation());
    switch (spriteTouched)
    {
        case eButton0:
            button0Pressed();
            break;
        case eButton1:
            button1Pressed();
            break;
        case eButton2:
            button2Pressed();
            break;
        case eButton3:
            button3Pressed();
            break;
    }
    return true;
}

void Game::button0Pressed()
{
    auto square2 = Unit::create("PinkSquare.png");
    square2->setPosition(100,100);
    square2->setAnchorPoint(Vec2(0.5, 0.5));
    unitManager->addChild(square2);
}

void Game::button1Pressed()
{
    auto homeBase = mainSprite->getChildByTag(eHomeBase);
    Vec2 homeLocation = homeBase->getPosition();
    for (auto unit : unitManager->getChildren())
    {
        auto action = MoveTo::create(3, homeLocation);
        unit->runAction(action);
    }
}
void Game::button2Pressed()
{
    auto enemyBase = mainSprite->getChildByTag(eEnemyBase);
    Vec2 enemyLocation = enemyBase->getPosition();
    for (auto unit : unitManager->getChildren())
    {
        auto action = MoveTo::create(3, enemyLocation);
        unit->runAction(action);
    }
}

void Game::button3Pressed()
{
    auto square2 = Unit::create("GreenSquare.png");
    square2->setPosition(500,200);
    square2->setAnchorPoint(Vec2(0.5, 0.5));
    unitManager->addChild(square2);
}

int Game::touchedSpriteBottom(Vec2 location)
{
    for (auto node : bottomSprite->getChildren())
    {
        if (node->getBoundingBox().containsPoint(location))
            return node->getTag();
    }
    return -1;
}