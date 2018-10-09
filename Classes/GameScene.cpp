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
    SpriteFrame* bottomFrame = SpriteFrame::create("Grey.png", Rect(0.0, 0.0, 1334.0, 200.0));
    SpriteFrame* mainFrame = SpriteFrame::create("Blue.png", Rect(0.0, 0.0, 1334.0, 550.0));
    
    auto bottomSprite = Sprite::create();
    bottomSprite->setSpriteFrame(bottomFrame);
    bottomSprite->setPosition(0, 0);
    bottomSprite->setAnchorPoint(Vec2(0.0, 0.0));
    auto mainSprite = Sprite::create();
    mainSprite->setSpriteFrame(mainFrame);
    mainSprite->setPosition(0, 200);
    mainSprite->setAnchorPoint(Vec2(0.0, 0.0));
    auto square = Sprite::create("Square.png");
    square->setPosition(0,0);
    square->setAnchorPoint(Vec2(0.5, 0.5));
    bottomSprite->addChild(square);

    this->addChild(bottomSprite, 0);
    this->addChild(mainSprite, 0);
    
    return true;
}