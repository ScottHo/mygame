#include "GameScene.h"

#include <iostream>
#include <string>
#include <memory>
#include <algorithm>
#include <random>
#include <fstream>
#include <sstream>
#include <iomanip>
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
    this->scheduleUpdate();
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
    float bottomFrameHeight = fWindowHeight*(1.0/3.0);
    float topFrameHeight = fWindowHeight/(2.0/3.0); // 1400 width, 500 height
    float contextFrameHeight = bottomFrameHeight;
    float contextFrameWidth = fWindowWidth * (12.0/15.0);
    float infoFrameHeight = bottomFrameHeight;
    float infoFrameWidth = fWindowWidth - contextFrameWidth;
    SpriteFrame* contextSpriteFrame = SpriteFrame::create("Empty.png", Rect(0.0, 0.0, contextFrameWidth, bottomFrameHeight));
	contextFrame = Sprite::create();
    contextFrame->setSpriteFrame(contextSpriteFrame);
    contextFrame->setPosition(0, 0);
    contextFrame->setAnchorPoint(Vec2(0.0, 0.0));
    this->addChild(contextFrame, 0);

    SpriteFrame* infoSpriteFrame = SpriteFrame::create("Empty.png", Rect(0.0, 0.0, infoFrameWidth, infoFrameHeight));
    infoFrame = Sprite::create();
    infoFrame->setSpriteFrame(infoSpriteFrame);
    infoFrame->setPosition(contextFrameWidth, 0);
    infoFrame->setAnchorPoint(Vec2(0.0, 0.0));
    this->addChild(infoFrame, 0);

    SpriteFrame* topSpriteFrame = SpriteFrame::create("Empty.png", Rect(0.0, 0.0, fWindowWidth, topFrameHeight));
    topFrame = Sprite::create();
    topFrame->setSpriteFrame(topSpriteFrame);
    topFrame->setPosition(0, bottomFrameHeight);
    topFrame->setAnchorPoint(Vec2(0.0, 0.0));
    this->addChild(topFrame, 0);

    SpriteFrame* emptyWindowFrame = SpriteFrame::create("Empty.png", Rect(0.0, 0.0, fWindowWidth, fWindowHeight));
    globalFrame = Sprite::create();
    globalFrame->setSpriteFrame(emptyWindowFrame);
    globalFrame->setPosition(0, 0);
    globalFrame->setAnchorPoint(Vec2(0.0, 0.0));
    this->addChild(globalFrame, 0);

    towerManager = Sprite::create();
    towerManager->setSpriteFrame(emptyWindowFrame);
    towerManager->setPosition(0, 0);
    towerManager->setAnchorPoint(Vec2(0.0, 0.0));
    this->addChild(towerManager, 0);

    SpriteFrame* gameFrame = SpriteFrame::create("Blue.png", Rect(0.0, 0.0, fWindowWidth, topFrameHeight));
    gameView = Sprite::create();
    gameView->setSpriteFrame(gameFrame);
    gameView->setPosition(0, 0);
    gameView->setAnchorPoint(Vec2(0,0));
    gameView->setName("GameView");
    topFrame->addChild(gameView);

    SpriteFrame* tileFrame = SpriteFrame::create("Empty.png", Rect(0.0, 0.0, fWindowWidth, topFrameHeight));
    tileManager = Sprite::create();
    tileManager->setSpriteFrame(tileFrame);
    tileManager->setPosition(0, 0);
    tileManager->setAnchorPoint(Vec2(0,0));
    tileManager->setName("TileManager");
    gameView->addChild(tileManager);

    float rows = 5.0;
    float columns =  10.0;
    float tileWidth = fWindowWidth/columns;
    std::cout << tileWidth << " = tilesize\n";
    float scale = tileWidth/100.0;
    int midPoint = 5;
    int tileCounter = 0;

    for (int i = 0; i < columns; ++i)
    {
        for (int j = 0; j < rows; ++j)
        {
            if ((i == columns-1))
                continue;

            Sprite* tile;
            if (((j == 0 or j == rows-1.0) and i < midPoint) or (i == midPoint) or ((j == 1 or j == 3) and i > midPoint))
            {
                tile = Sprite::create("Path.png");
                tile->setName("Path");
            }
            else
            {
                tile = Sprite::create("Grass.png");
                tile->setName("Grass");
            }

            tile->setPosition(i*tileWidth, j*tileWidth);
            tile->setAnchorPoint(Vec2(0,0));
            tile->setScale(scale, scale);
            tileManager->addChild(tile, 0, tileCounter);
            ++tileCounter;
        }
    }

    // Enemy Building
    Sprite* homeBase = Sprite::create("Building.png");
    homeBase->setPosition((columns-1)*tileWidth, 0);
    homeBase->setAnchorPoint(Vec2(0,0));
    homeBase->setScale(scale, scale);
    tileManager->addChild(homeBase);

    SpriteFrame* lettersFrame = SpriteFrame::create("Grey.png", Rect(0.0, 0.0, contextFrameWidth, contextFrameHeight));
    lettersView = Sprite::create();
    lettersView->setSpriteFrame(lettersFrame);
    lettersView->setPosition(0,0);
    lettersView->setAnchorPoint(Vec2(0.0,0.0));
    lettersView->setName("LettersView");
    contextFrame->addChild(lettersView, 1);

    SpriteFrame* buildingFrame = SpriteFrame::create("White.png", Rect(0.0, 0.0, contextFrameWidth, contextFrameHeight));
    buildingView = Sprite::create();
    buildingView->setSpriteFrame(buildingFrame);
    buildingView->setPosition(0,0);
    buildingView->setAnchorPoint(Vec2(0.0,0.0));
    buildingView->setName("BuildingView");
    contextFrame->addChild(buildingView, 0);

    SpriteFrame* loadingZoneFrame = SpriteFrame::create("Grey.png", Rect(0.0, 0.0, 100.0, 100.0));
    loadingZone = Sprite::create();
    loadingZone->setSpriteFrame(loadingZoneFrame);
    loadingZone->setPosition(contextFrameWidth/2.0, contextFrameHeight/2.0);
    loadingZone->setAnchorPoint(Vec2(0.0,0.0));
    loadingZone->setName("loadingZone");
    buildingView->addChild(loadingZone, 0);

    auto doneButton = ui::Button::create("DoneButtonUnclicked.png", "DoneButtonclicked.png", "DoneButtonUnclicked.png");
    doneButton->setPosition(Vec2(contextFrameWidth*9.4/12.0, bottomFrameHeight*(4.0/7.0)));
    doneButton->setAnchorPoint(Vec2(0.0,0.0));
    doneButton->setScale(.85, 1.0);
    doneButton->addTouchEventListener(CC_CALLBACK_2(Game::onDone, this));
    buildingView->addChild(doneButton);

    SpriteFrame* infoBottomFrame = SpriteFrame::create("Black.png", Rect(0.0, 0.0, infoFrameWidth, infoFrameHeight));
    infoView = Sprite::create();
    infoView->setSpriteFrame(infoBottomFrame);
    infoView->setPosition(0,0);
    infoView->setAnchorPoint(Vec2(0.0,0.0));
    infoView->setName("InfoView");
    infoFrame->addChild(infoView);

    auto moneyPrefix = Label::createWithSystemFont("$", "Arial", 16);
    moneyPrefix->setPosition(Vec2(infoFrameWidth/5, infoFrameHeight*4.0/5.0));
    moneyPrefix->setAnchorPoint(Vec2(0, 0));
    infoView->addChild(moneyPrefix);
    moneyLabel = Label::createWithSystemFont("0", "Arial", 16);
    moneyLabel->setPosition(Vec2(infoFrameWidth*2.0/5.0, infoFrameHeight*4.0/5.0));
    moneyLabel->setAnchorPoint(Vec2(0, 0));
    infoView->addChild(moneyLabel);

    auto timePrefix = Label::createWithSystemFont("0:", "Arial", 16);
    timePrefix->setPosition(Vec2(infoFrameWidth/5, infoFrameHeight*3.0/5.0));
    timePrefix->setAnchorPoint(Vec2(0, 0));
    infoView->addChild(timePrefix);
    timeLabel = Label::createWithSystemFont("", "Arial", 16);
    std::stringstream ss;
    ss << std::fixed << std::setprecision(0) << levelTimer;
    timeLabel->setString(ss.str());
    timeLabel->setPosition(Vec2(infoFrameWidth*2.0/5.0, infoFrameHeight*3.0/5.0));
    timeLabel->setAnchorPoint(Vec2(0, 0));
    infoView->addChild(timeLabel);

    SpriteFrame* emptyContextFrame = SpriteFrame::create("Empty.png", Rect(0.0, 0.0, contextFrameWidth, contextFrameHeight));
    fieldManager = Sprite::create();
    fieldManager->setSpriteFrame(emptyContextFrame);
    fieldManager->setPosition(0,0);
    fieldManager->setAnchorPoint(Vec2(0.0,0.0));
    fieldManager->setName("FieldManager");
    lettersView->addChild(fieldManager, 0, eFieldManager);

    handManager = Sprite::create();
    handManager->setSpriteFrame(emptyContextFrame);
    handManager->setPosition(0,0);
    handManager->setAnchorPoint(Vec2(0.0,0.0));
    handManager->setName("HandManager");
    lettersView->addChild(handManager, 0, eHandManager);

    letterManager = Sprite::create();
    letterManager->setSpriteFrame(emptyContextFrame);
    letterManager->setPosition(0,0);
    letterManager->setAnchorPoint(Vec2(0.0,0.0));
    letterManager->setName("LetterManager");
    lettersView->addChild(letterManager, 0, eLetterManager);

    auto submitButton = ui::Button::create("SubmitButtonUnclicked.png", "SubmitButtonClicked.png", "SubmitButtonUnclicked.png");
    submitButton->setPosition(Vec2(contextFrameWidth*9.4/12.0, bottomFrameHeight*(4.0/7.0)));
    submitButton->setAnchorPoint(Vec2(0.0,0.0));
    submitButton->setScale(.85, 1.0);
    submitButton->addTouchEventListener(CC_CALLBACK_2(Game::onSubmit, this));
    lettersView->addChild(submitButton, 0, eSubmitButton);

    for (int i = 0; i<numLetters; ++i)
    {
        double count = i + .70;
        auto letterHolder = Sprite::create(letterHolderImage);
        auto holder = new Holder(false);
        letterHolder->setUserObject(holder);
        letterHolder->setPosition(contextFrameWidth*count/12.0, bottomFrameHeight*(2.0/7.0));
        letterHolder->setAnchorPoint(Vec2(0.5, 0.5));
        letterHolder->setScale(1.3, 1.3);
        handManager->addChild(letterHolder, 0, i);
    }

    for (int i = 0; i<numLetters; ++i)
    {
        double count = i + .70;
        auto letterHolder = Sprite::create(letterHolderImage);
        auto holder = new Holder(false);
        letterHolder->setUserObject(holder);
        letterHolder->setPosition(contextFrameWidth*count/12.0, bottomFrameHeight*(5.0/7.0));
        letterHolder->setAnchorPoint(Vec2(0.5, 0.5));
        letterHolder->setScale(1.3, 1.3);
        fieldManager->addChild(letterHolder, 0, i);
    }
}

void Game::update(float delta)
{
    if (doCountdown)
    {
        levelTimer -= delta;
        std::stringstream ss;
        ss << std::fixed << std::setprecision(0) << levelTimer;
        timeLabel->setString(ss.str());
        if (levelTimer <= 0.05)
        {
            wordPhaseDone();
        }
    }
}

void Game::wordPhaseDone()
{
    doCountdown = false;
    Tower* t = createTower(longestWord);
    Sprite* newTower = Sprite::create("BaseTower.png");
    newTower->setPosition(buildingView->convertToWorldSpace(loadingZone->getPosition()));
    newTower->setAnchorPoint(Vec2(0,0));
    newTower->setUserObject(t);
    newTower->setName("New");
    towerManager->addChild(newTower);
    buildingView->setLocalZOrder(1);
    currentPhase = stBuild;

    longestWord = 0;
    sValidWord = "";
    bIsValidWord = false;
}

void Game::buildPhaseDone()
{
    currentPhase = stKill;
}

Tower* Game::createTower(unsigned int level)
{
    Tower* tower = new Tower(1, 1, 1, false);
    return tower;
}

void Game::dealTiles()
{
    std::string randomLetters = generateRandomLetters();
    for (int i = 0; i<numLetters; ++i)
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
        sprite->setScale(1.2, 1.2);
        letterManager->addChild(sprite, 0, i);
    }
}
std::string Game::generateRandomLetters()
{
    std::random_device random_device;
    std::mt19937 engine{random_device()};
    std::uniform_int_distribution<int> dist(4, 5);
    int numVowels = dist(engine);
    int numConsonants = numLetters - numVowels;

    std::string consonant = "BBBBBCCCCCCCCCDDDDDDDDDFFFGGGGGGGHHHHHJJLLLLLLLLLLLMMMMMMNNNNNNNNNNNNNNPPPPPPQRRRRRRRRRRRRRRSSSSSSSSSSSSSSSSSTTTTTTTTTTTTTTTTVVVXXZZ";
    std::string vowels = "AAAAAEEEEEEIIIIIOOOOUU";
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
    if (formattedWord.length() > 2 and 
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
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener,globalFrame);
}

void Game::onSubmit(Ref* sender, ui::Widget::TouchEventType type)
{
    switch (type)
    {
        case ui::Widget::TouchEventType::BEGAN:
            break;
        case ui::Widget::TouchEventType::ENDED:
            if (bIsValidWord)
            {
                if (std::find(vWordsUsed.begin(), vWordsUsed.end(), sValidWord) == vWordsUsed.end())
                {
                    vWordsUsed.push_back(sValidWord);
                    if (sValidWord.length() == 3)
                        money += 1;
                    else if (sValidWord.length() ==4)
                        money += 2;
                    else if (sValidWord.length() ==5)
                        money += 3;
                    else if (sValidWord.length() ==6)
                        money += 5;
                    else if (sValidWord.length() ==7)
                        money += 7;
                    else if (sValidWord.length() ==8)
                        money += 10;
                    else 
                        money += 20;
                    moneyLabel->setString(std::to_string(money));
                    if (sValidWord.length() > longestWord)
                        longestWord = sValidWord.length();
                }
            }
            break;
        default:
            break;
    }
}

void Game::onDone(Ref* sender, ui::Widget::TouchEventType type)
{
    switch (type)
    {
        case ui::Widget::TouchEventType::BEGAN:
            break;
        case ui::Widget::TouchEventType::ENDED:
            if (bIsValidWord)
            {
                buildPhaseDone();
            }
            break;
        default:
            break;
    }
}


void Game::updateMoney()
{

}

bool Game::onTouchStart(Touch* touch, Event* event)
{

    auto currentLocation = touch->getLocation();
    if (currentPhase == stWord)
    {
        int spriteTouched = touchedLetter(currentLocation);
        if (spriteTouched > -1)
        {
            if (!bLetterPickedUp)
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
    }
    else if (currentPhase == stBuild)
    {
        if (loadingZone->getBoundingBox().containsPoint(currentLocation))
        {
            bTowerPickedUp = true;
            originalLocation = buildingView->convertToWorldSpace(loadingZone->getPosition());
            currentTower = towerManager->getChildByName("New");
            lastTouchLocation = currentLocation;
        }
        int spriteTouched = touchedLetter(currentLocation);
        if (spriteTouched > -1)
        {
            std::cout << "Tower Touched\n";
        }
    }
    return true;
}

bool Game::onTouchMove(Touch* touch, Event* event)
{
    if (currentPhase == stWord)
    {
        if (bLetterPickedUp)
        {
            lastTouchLocation = touch->getLocation();
            currentLetter->setPosition(currentLetter->getPosition() + touch->getDelta());
        }
    }
    else if (currentPhase == stBuild)
    {
        if (bTowerPickedUp)
        {
            lastTouchLocation = touch->getLocation();
            currentTower->setPosition(currentTower->getPosition() + touch->getDelta());
        }

    }
    return true;
}
bool Game::onTouchEnd(Touch* touch, Event* event)
{
    if (currentPhase == stWord)
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
                    if (currentHolder->getParent()->getName() == "FieldManager")
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
                    if (currentHolder->getParent()->getName() == "FieldManager")
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
    }
    else if (currentPhase == stBuild)
    {
        if (bTowerPickedUp)
        {
            int tileTouched = touchedTile(lastTouchLocation);
            if (tileTouched > -1)
            {
                auto tile = tileManager->getChildByTag(tileTouched);
                Vec2 newLocation = tileManager->convertToWorldSpace(tile->getPosition());
                auto action = MoveTo::create(0.2, newLocation);
                currentTower->runAction(action);
            }
            else
            {
                auto action = MoveTo::create(0.2, originalLocation);
                currentTower->runAction(action);

            }
            bTowerPickedUp = false;

        }
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

int Game::touchedTower(Vec2 location)
{
    for (auto node : towerManager->getChildren())
    {
        if (node->getBoundingBox().containsPoint(location))
            return node->getTag();
    }
    return -1;
}

int Game::touchedTile(Vec2 location)
{
    for (auto node : tileManager->getChildren())
    {
        if (node->getBoundingBox().containsPoint(location))
            return node->getTag();
    }
    return -1;
}