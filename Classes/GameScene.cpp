#include "GameScene.h"
#include "Letter.h"
#include "Holder.h"
#include <iostream>
#include <string>
#include <memory>
#include <algorithm>
#include <random>
#include <fstream>

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
    loadAllWords();
    setupUi();
    setupEvents();

    dealTiles();
    return true;
}

void Game::clearTiles()
{
    for (auto node : letterManager->getChildren())
    {
        Letter::clearFromNode(node);
    }
}

void Game::cleanup()
{
    for (auto node : letterManager->getChildren())
    {
        Holder::clearFromNode(node);
    }
}

void Game::setupUi()
{
    // Main frames
    float bottomFrameHeight = fWindowHeight*(3.0/4.0);
    float topFrameHeight = fWindowHeight/4.0;
    SpriteFrame* bottomFrame = SpriteFrame::create("Grey.png", Rect(0.0, 0.0, fWindowWidth, bottomFrameHeight));
    SpriteFrame* topFrame = SpriteFrame::create("Blue.png", Rect(0.0, 0.0, fWindowWidth, topFrameHeight));
	bottomSprite = Sprite::create();
    bottomSprite->setSpriteFrame(bottomFrame);
    bottomSprite->setPosition(0, 0);
    bottomSprite->setAnchorPoint(Vec2(0.0, 0.0));

    topSprite = Sprite::create();
    topSprite->setSpriteFrame(topFrame);
    topSprite->setPosition(0, bottomFrameHeight);
    topSprite->setAnchorPoint(Vec2(0.0, 0.0));

    this->addChild(bottomSprite, 0);
    this->addChild(topSprite, 0);

    SpriteFrame* emptyBottomFrame = SpriteFrame::create("Empty.png", Rect(0.0, 0.0, fWindowWidth, bottomFrameHeight));


    fieldManager = Sprite::create();
    fieldManager->setSpriteFrame(emptyBottomFrame);
    fieldManager->setPosition(0,0);
    fieldManager->setAnchorPoint(Vec2(0.0,0.0));
    fieldManager->setName("Field");
    bottomSprite->addChild(fieldManager, 0, eFieldManager);

    handManager = Sprite::create();
    handManager->setSpriteFrame(emptyBottomFrame);
    handManager->setPosition(0,0);
    handManager->setAnchorPoint(Vec2(0.0,0.0));
    handManager->setName("Hand");
    bottomSprite->addChild(handManager, 0, eHandManager);

    letterManager = Sprite::create();
    letterManager->setSpriteFrame(emptyBottomFrame);
    letterManager->setPosition(0,0);
    letterManager->setAnchorPoint(Vec2(0.0,0.0));
    letterManager->setName("Letter");
    bottomSprite->addChild(letterManager, 0, eLetterManager);

    for (int i = 0; i<10; ++i)
    {
        double count = i + 2.0;
        auto letterHolder = Sprite::create(letterHolderImage);
        auto holder = new Holder(false);
        letterHolder->setUserObject(holder);
        letterHolder->setPosition(fWindowWidth*count/13.0, bottomFrameHeight*(1.0/4.0));
        letterHolder->setAnchorPoint(Vec2(0.5, 0.5));
        handManager->addChild(letterHolder, 0, i);
    }

    for (int i = 0; i<10; ++i)
    {
        double count = i + 2.0;
        auto letterHolder = Sprite::create(letterHolderImage);
        auto holder = new Holder(false);
        letterHolder->setUserObject(holder);
        letterHolder->setPosition(fWindowWidth*count/13.0, bottomFrameHeight*(3.0/4.0));
        letterHolder->setAnchorPoint(Vec2(0.5, 0.5));
        fieldManager->addChild(letterHolder, 0, i);
    }

}

void Game::dealTiles()
{
    std::string randomLetters = generateRandomLetters();
    for (int i = 0; i<10; ++i)
    {
        auto letter = new Letter(randomLetters.at(i), 1);
        std::string filename = "";
        filename += letter->value();
        filename += ".png";
        std::cout << filename << "\n";
        auto sprite = Sprite::create(filename);
        auto holder = handManager->getChildByTag(i);
        sprite->setUserObject(letter);
        sprite->setPosition(holder->getPosition());
        sprite->setAnchorPoint(Vec2(0.5, 0.5));
        letterManager->addChild(sprite, 0, i);
    }
}

std::string Game::generateRandomLetters()
{
    std::random_device random_device;
    std::mt19937 engine{random_device()};
    std::uniform_int_distribution<int> dist(3, 6);
    int numVowels = dist(engine);
    int numConsonants = 10 - numVowels;

    std::string consonant = "BBBBCCCCCDDDDFFFGGGGHHHHJJJLLLLLMMMMMMNNNNNPPPPQQQRRRRSSSSSTTTTVVVWWWXXXZZZ";
    std::string vowels = "AAAAEEEEIIIIOOOOUUU";
    std::string ret;

    for (int i = 0; i < numConsonants; i++)
    {
        std::mt19937 engine2{random_device()};
        std::uniform_int_distribution<int> dist2(0, consonant.length() - 1);
        ret += consonant.at(dist2(engine));    
    }
    for (int i = 0; i < numVowels; i++)
    {
        std::mt19937 engine2{random_device()};
        std::uniform_int_distribution<int> dist2(0, vowels.length() - 1);
        ret += vowels.at(dist2(engine));    
    }
    std::random_shuffle(ret.begin(), ret.end());
    return ret;    
}

void Game::updateCurrentWord(char letter, int index)
{
    currentWord[index] = letter;
}

void Game::updateValidWord()
{
    std::string formattedWord;
    for (char letter : currentWord)
    {
        if (letter != '-')
        {
            formattedWord += letter;
        }
    }
    if (formattedWord.length() > 2 && 
        std::find(vWords.begin(), vWords.end(), formattedWord) != vWords.end())
    {
        sValidWord = formattedWord;
        bIsValidWord = true;
        std::cout << sValidWord << "\n";
    }
    else
    {
        sValidWord = "";
        bIsValidWord = false;
    }
}

void Game::loadAllWords()
{
    std::ifstream file("Resources/words.txt");
    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            vWords.push_back(line);
        }
        file.close();
    }
}

void Game::setupEvents()
{
    auto touchListener = EventListenerTouchOneByOne::create();  
    touchListener->onTouchBegan = CC_CALLBACK_2(Game::onTouchStart, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(Game::onTouchMove, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(Game::onTouchEnd, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener,bottomSprite);
}

bool Game::onTouchStart(Touch* touch, Event* event)
{
    if (!bLetterPickedUp)
    {
        auto currentLocation = touch->getLocation();
        int spriteTouched = touchedLetter(currentLocation);
        if (spriteTouched > -1)
        {
            bLetterPickedUp = true;
            int fieldTouched = touchedFieldHolder(currentLocation);
            int handTouched = touchedHandHolder(currentLocation);
            if (fieldTouched > -1)
            {
                originalLocation = fieldManager->getChildByTag(fieldTouched)->getPosition();
                currentHolder = fieldManager->getChildByTag(fieldTouched);
            }
            else if (handTouched > -1)
            {
                originalLocation = handManager->getChildByTag(handTouched)->getPosition();
                currentHolder = handManager->getChildByTag(handTouched);
            }
            else
            {
                std::cout << "something went wrong\n";
            }
            lastTouchLocation = currentLocation;
            currentLetter = letterManager->getChildByTag(spriteTouched);
        }
    }
    return true;
}

bool Game::onTouchMove(Touch* touch, Event* event)
{
    if (bLetterPickedUp)
    {
        lastTouchLocation = touch->getLocation();
        currentLetter->setPosition(currentLetter->getPosition() + touch->getDelta());
    }
    return true;
}
bool Game::onTouchEnd(Touch* touch, Event* event)
{
    if (bLetterPickedUp)
    {
        int fieldTouched = touchedFieldHolder(lastTouchLocation);
        int handTouched = touchedHandHolder(lastTouchLocation);

        if (fieldTouched > -1)
        {
            auto holder = fieldManager->getChildByTag(fieldTouched);
            if (!Holder::getValueFromNode(holder))
            {
                Holder::setValueFromNode(holder, true);
                Holder::setValueFromNode(currentHolder, false);
                std::cout << holder->getTag() << " has tile\n";
                std::cout << currentHolder->getTag() << " has no tile\n";
                if (currentHolder->getParent()->getName() == "Field")
                    updateCurrentWord('-', currentHolder->getTag());
                currentHolder = holder;
                auto action = MoveTo::create(0.2, holder->getPosition());
                currentLetter->runAction(action);
                updateCurrentWord(Letter::getLetterFromNode(currentLetter), fieldTouched);
            }
            else
            {
                auto action = MoveTo::create(0.2, originalLocation);
                currentLetter->runAction(action);
            }
        }
        else if (handTouched > -1)
        {
            auto holder = handManager->getChildByTag(handTouched);
            if (!Holder::getValueFromNode(holder))
            {
                Holder::setValueFromNode(holder, true);
                Holder::setValueFromNode(currentHolder, false);
                if (currentHolder->getParent()->getName() == "Field")
                    updateCurrentWord('-', currentHolder->getTag());
                currentHolder = holder;
                auto action = MoveTo::create(0.2, holder->getPosition());
                currentLetter->runAction(action);
            }
            else
            {
                auto action = MoveTo::create(0.2, originalLocation);
                currentLetter->runAction(action);
            }
        }
        else
        {
            auto action = MoveTo::create(0.2, originalLocation);
            currentLetter->runAction(action);
        }
        bLetterPickedUp = false;
        updateValidWord();
    }
    return true;
}

int Game::touchedFieldHolder(Vec2 location)
{
    for (auto node : fieldManager->getChildren())
    {
        if (node->getBoundingBox().containsPoint(location))
        {
            return node->getTag();
        }
    }
    return -1;
}

int Game::touchedHandHolder(Vec2 location)
{
    for (auto node : handManager->getChildren())
    {
        if (node->getBoundingBox().containsPoint(location))
        {
            return node->getTag();
        }
    }
    return -1;
}

int Game::touchedLetter(Vec2 location)
{
    for (auto node : letterManager->getChildren())
    {
        if (node->getBoundingBox().containsPoint(location))
            return node->getTag();
    }
    return -1;
}