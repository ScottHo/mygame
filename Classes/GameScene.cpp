#include "GameScene.h"

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

    auto button0 = Sprite::create("Square.png");
    button0->setPosition(fWindowWidth/4.0, bottomFrameHeight/2.0);
    button0->setAnchorPoint(Vec2(0.5, 0.5));
    bottomSprite->addChild(button0, 0, eButton0);
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(Game::onButton0Touch, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener,button0);

    auto button1 = Sprite::create("Square.png");
    button1->setPosition(fWindowWidth*(2.0/4.0), bottomFrameHeight/2.0);
    button1->setAnchorPoint(Vec2(0.5, 0.5));
    bottomSprite->addChild(button1, 0, eButton1);
    auto touchListener2 = EventListenerTouchOneByOne::create();
    touchListener2->onTouchBegan = CC_CALLBACK_2(Game::onButton1Touch, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener2,button1);

    auto button2 = Sprite::create("Square.png");
    button2->setPosition(fWindowWidth*(3.0/4.0), bottomFrameHeight/2.0);
    button2->setAnchorPoint(Vec2(0.5, 0.5));
    bottomSprite->addChild(button2, 0, eButton2);

}

bool Game::onButton0Touch(Touch* touch, Event* event)
{
    auto square2 = Sprite::create("Square.png");
    square2->setPosition(100,100);
    square2->setAnchorPoint(Vec2(0.5, 0.5));
    mainSprite->addChild(square2);
    return true;
}

bool Game::onButton1Touch(Touch* touch, Event* event)
{
    auto square2 = Sprite::create("Square.png");
    square2->setPosition(200,200);
    square2->setAnchorPoint(Vec2(0.5, 0.5));
    mainSprite->addChild(square2);
    return true;
}
