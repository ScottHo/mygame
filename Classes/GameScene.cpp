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
    auto scene = Scene::createWithPhysics();
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
    loadAllWords();
    setupUi();
    setupEvents();
    this->scheduleUpdate();
    killPhaseDone();
    return true;
}

void Game::clearTiles()
{
    bLetterPickedUp = false;
    currentLetter = nullptr;
    for (auto holder : fieldManager->getChildren())
    {
        HolderNode* tmp = dynamic_cast<HolderNode*>(holder);
        tmp->setValue(false);
    }
    for (auto holder : handManager->getChildren())
    {
        HolderNode* tmp = dynamic_cast<HolderNode*>(holder);
        tmp->setValue(false);
    }
    letterManager->removeAllChildrenWithCleanup(true);
}

void Game::clearUnits()
{
    for (auto tower : towerManager->getChildren())
    {
        TowerNode* tmp = dynamic_cast<TowerNode*>(tower);
        tmp->clearTargets();
    }
    unitManager->removeAllChildrenWithCleanup(true);
}

void Game::clearTowers()
{
    bTowerPickedUp = false;
    currentTower = nullptr;
    towerManager->removeAllChildrenWithCleanup(true);
}

void Game::clearHolders()
{
    fieldManager->removeAllChildrenWithCleanup(true);
    handManager->removeAllChildrenWithCleanup(true);
}   

void Game::cleanup()
{
    clearTiles();
    clearTowers();
    clearUnits();
    clearHolders();
}

void Game::setupUi()
{
    // Main frames
    fWindowHeight = Director::getInstance()->getVisibleSize().height;
    fWindowWidth = Director::getInstance()->getVisibleSize().width;
    // Create a 14 by 7 tile

    float tileSize;
    if (fWindowHeight*2.0 > fWindowWidth)
    {
        tileSize = fWindowWidth / numColumns;
    }
    else
    {
        tileSize = fWindowHeight / numRows;
    }

    Sprite* baseTile = Sprite::create("Grass.png");
    float originalSize = baseTile->getContentSize().width;
    scale = tileSize/originalSize;
    std::cout << "tileSize " << tileSize << ", originalSize " << originalSize << "\n";

    float gameFrameHeight = gameRows * tileSize; 
    float gameFrameWidth = gameColumns * tileSize;
    float infoFrameHeight = gameRows * tileSize;
    float infoFrameWidth = (numColumns - gameColumns) * tileSize;
    std::cout << "windowFrame = " << fWindowWidth << " " << fWindowHeight << "\n";
    std::cout << "gameFrame = " << gameFrameWidth << " " << gameFrameHeight << "\n";
    std::cout << "infoFrame = " << infoFrameWidth << " " << infoFrameHeight << "\n";

    SpriteFrame* gameSpriteFrame = SpriteFrame::create("Empty.png", Rect(0.0, 0.0, gameFrameWidth, gameFrameHeight));
	gameFrame = Sprite::create();
    gameFrame->setSpriteFrame(gameSpriteFrame);
    gameFrame->setPosition(0, 0);
    gameFrame->setAnchorPoint(Vec2(0.0, 0.0));
    this->addChild(gameFrame, 0);

    SpriteFrame* lettersSpriteFrame = SpriteFrame::create("Beige.png", Rect(0.0, 0.0, gameFrameWidth, gameFrameHeight));
    lettersFrame = Sprite::create();
    lettersFrame->setSpriteFrame(lettersSpriteFrame);
    lettersFrame->setPosition(0, 0);
    lettersFrame->setAnchorPoint(Vec2(0,0));
    lettersFrame->setName("LettersFrame");
    this->addChild(lettersFrame, 1);

    SpriteFrame* infoSpriteFrame = SpriteFrame::create("Grey.png", Rect(0.0, 0.0, infoFrameWidth, infoFrameHeight));
    infoFrame = Sprite::create();
    infoFrame->setSpriteFrame(infoSpriteFrame);
    infoFrame->setPosition(gameFrameWidth, 0.0);
    infoFrame->setAnchorPoint(Vec2(0.0, 0.0));
    infoFrame->setName("InfoFrame");
    this->addChild(infoFrame, 0);

    SpriteFrame* emptyWindowFrame = SpriteFrame::create("Empty.png", Rect(0.0, 0.0, fWindowWidth, fWindowHeight));
    globalFrame = Sprite::create();
    globalFrame->setSpriteFrame(emptyWindowFrame);
    globalFrame->setPosition(0, 0);
    globalFrame->setAnchorPoint(Vec2(0.0, 0.0));
    this->addChild(globalFrame, 2);

    towerManager = Sprite::create();
    towerManager->setSpriteFrame(emptyWindowFrame);
    towerManager->setPosition(0, 0);
    towerManager->setAnchorPoint(Vec2(0.0, 0.0));
    globalFrame->addChild(towerManager, 0);

    gridManager = Sprite::create();
    gridManager->setSpriteFrame(gameSpriteFrame);
    gridManager->setPosition(0, 0);
    gridManager->setAnchorPoint(Vec2(0,0));
    gridManager->setName("GridManager");
    gameFrame->addChild(gridManager);

    unitManager = Sprite::create();
    unitManager->setSpriteFrame(gameSpriteFrame);
    unitManager->setPosition(0, 0);
    unitManager->setAnchorPoint(Vec2(0,0));
    unitManager->setName("UnitManager");
    gameFrame->addChild(unitManager);

    int tileCounter = 0;
    int map[(int)gameRows][(int)gameColumns] =
    {{2,  0,  0,  1, -1, -1, 10,  0,  0,  9},
     {0, -1, -1, -1, -1, -1, -1, -1, -1,  0},
     {0, -1, -1,  5,  0,  0,  6, -1, -1,  0},
     {0, -1, -1,  0, -1, -1,  0, -1, -1,  0},
     {0, -1, -1,  0, -1, -1,  0, -1, -1,  0},
     {3,  0,  0,  4, -1, -1,  7,  0,  0,  8}};

    int currentRow, currentColumn;
    for (float i = 0; i < gameColumns; ++i)
    {
        for (float j = 0; j < gameRows; ++j)
        {
            Sprite* tile;
            currentRow = gameRows - 1 - (int)j;
            currentColumn = (int)i;
            if (map[currentRow][currentColumn] == -1)
            {
                tile = Sprite::create("Grass.png");
                tile->setName("Grass");
                tile->setTag(1000+tileCounter);

            }
            else
            {
                
                tile = Sprite::create("Path.png");
                tile->setName("Path");
                if (map[currentRow][currentColumn] > 0)
                {
                    int tag = map[currentRow][currentColumn];
                    tile->setTag(tag);
                }
                else
                {
                    tile->setTag(1000+tileCounter);
                }
            }

            tile->setPosition(i*tileSize, j*tileSize);
            tile->setAnchorPoint(Vec2(0,0));
            tile->setScale(scale, scale); // somethines wrong with scaling...
            gridManager->addChild(tile);
            ++tileCounter;
        }
    }

    SpriteFrame* loadingZoneFrame = SpriteFrame::create("Black.png", Rect(0.0, 0.0, 100.0, 100.0));
    loadingZone = Sprite::create();
    loadingZone->setSpriteFrame(loadingZoneFrame);
    loadingZone->setPosition(infoFrameWidth*1.0/4.0, infoFrameHeight*(2.0/9.0));
    loadingZone->setAnchorPoint(Vec2(0.0,0.0));
    loadingZone->setName("loadingZone");
    loadingZone->setScale(scale);
    infoFrame->addChild(loadingZone, 0);

    doneButton = ui::Button::create("DoneButtonUnclicked.png", "DoneButtonClicked.png", "DoneButtonUnclicked.png");
    doneButton->setPosition(Vec2(infoFrameWidth*2.0/4.0, infoFrameHeight*(1.0/9.0)));
    doneButton->setAnchorPoint(Vec2(0.5, 0.5));
    doneButton->addTouchEventListener(CC_CALLBACK_2(Game::onDone, this));
    infoFrame->addChild(doneButton);

    moneyLabel = Label::createWithSystemFont("$0", "Arial", 16);
    moneyLabel->setString("$" + std::to_string(money));
    moneyLabel->setPosition(Vec2(infoFrameWidth*3.0/6.0, infoFrameHeight*(5.0/9.0)));
    moneyLabel->setAnchorPoint(Vec2(0.5, 0.5));
    infoFrame->addChild(moneyLabel);

    timeLabel = Label::createWithSystemFont("0:00", "Arial", 16);
    std::stringstream ss;
    ss << "0:"  << std::setw(2) << std::setfill('0') << std::fixed << std::setprecision(0) << levelTimer;
    timeLabel->setString(ss.str());
    timeLabel->setPosition(Vec2(infoFrameWidth*3.0/6.0, infoFrameHeight*6.0/9.0));
    timeLabel->setAnchorPoint(Vec2(0.5, 0.5));
    infoFrame->addChild(timeLabel);

    levelLabel = Label::createWithSystemFont("0", "Arial", 16);
    levelLabel->setString("Level " + std::to_string(currentLevel));
    levelLabel->setPosition(Vec2(infoFrameWidth*3.0/6.0, infoFrameHeight*7.0/9.0));
    levelLabel->setAnchorPoint(Vec2(0.5, 0.5));
    infoFrame->addChild(levelLabel);

    lifeLabel = Label::createWithSystemFont("", "Arial", 16);
    lifeLabel->setString(std::to_string(currentLife) + " Lives");
    lifeLabel->setPosition(Vec2(infoFrameWidth*3.0/6.0, infoFrameHeight*8.0/9.0));
    lifeLabel->setAnchorPoint(Vec2(0.5, 0.5));
    infoFrame->addChild(lifeLabel);

    auto baseLetterSprite = Sprite::create(fieldHolderImage);
    float letterWidth = baseLetterSprite->getContentSize().width;
    float fieldHolderHeight = baseLetterSprite->getContentSize().height * 1.35;

    float fieldHolderWidth = letterWidth*1.35;
    float numFieldHolders = gameFrameWidth/fieldHolderWidth;
    double margin = (gameFrameWidth - (fieldHolderWidth*17.0))/2.0;
    std::cout << margin << " = margin\n";
    std::cout << numFieldHolders << " = numFieldHolders\n";
    std::cout << fieldHolderWidth << " = fieldHolderWidth\n";


    auto fieldBackgroundSpriteFrame = SpriteFrame::create("Black.png", Rect(0.0, 0.0, fieldHolderWidth*19.5, fieldHolderHeight*1.42));
    fieldBackground = Sprite::create();
    fieldBackground->setSpriteFrame(fieldBackgroundSpriteFrame);
    fieldBackground->setAnchorPoint(Vec2(0.5, 0.5));
    fieldBackground->setPosition(gameFrameWidth/2.0, gameFrameHeight*(5.0/7.0));
    lettersFrame->addChild(fieldBackground);

    fieldManager = Sprite::create();
    fieldManager->setSpriteFrame(gameSpriteFrame);
    fieldManager->setPosition(0,0);
    fieldManager->setAnchorPoint(Vec2(0.0,0.0));
    fieldManager->setName("FieldManager");
    lettersFrame->addChild(fieldManager, 0, eFieldManager);

    handManager = Sprite::create();
    handManager->setSpriteFrame(gameSpriteFrame);
    handManager->setPosition(0,0);
    handManager->setAnchorPoint(Vec2(0.0,0.0));
    handManager->setName("HandManager");
    lettersFrame->addChild(handManager, 0, eHandManager);

    letterManager = Sprite::create();
    letterManager->setSpriteFrame(gameSpriteFrame);
    letterManager->setPosition(0,0);
    letterManager->setAnchorPoint(Vec2(0.0,0.0));
    letterManager->setName("LetterManager");
    lettersFrame->addChild(letterManager, 0, eLetterManager);

    submitButton = ui::Button::create("SubmitButtonUnclicked.png", "SubmitButtonClicked.png", "SubmitButtonUnclicked.png");
    submitButton->setPosition(Vec2(gameFrameWidth*3.0/9.0, gameFrameHeight*(1.0/7.0)));
    submitButton->setAnchorPoint(Vec2(0.0,0.0));
    submitButton->addTouchEventListener(CC_CALLBACK_2(Game::onSubmit, this));
    lettersFrame->addChild(submitButton, 0, eSubmitButton);

    startButton = ui::Button::create("StartButtonUnclicked.png", "StartButtonClicked.png", "StartButtonUnclicked.png");
    startButton->setPosition(Vec2(gameFrameWidth*6.0/9.0, gameFrameHeight*(1.0/7.0)));
    startButton->setAnchorPoint(Vec2(0.0,0.0));
    startButton->addTouchEventListener(CC_CALLBACK_2(Game::onStart, this));
    lettersFrame->addChild(startButton, 0, eSubmitButton);

    for (int i = 0; i<numLetters; ++i)
    {
        double count = i + 1.0;
        auto letterHolder = HolderNode::createHolder(letterHolderImage);
        letterHolder->setPosition(gameFrameWidth*count/10.0, gameFrameHeight*(3.0/7.0));
        letterHolder->setAnchorPoint(Vec2(0.5, 0.5));
        letterHolder->setScale(1.4, 1.4);
        handManager->addChild(letterHolder, 0, i);
    }


    for (int i = 0; i<numLetters*2-1; ++i)
    {
        double count = i;
        auto letterHolder = HolderNode::createHolder("blacktest.png");
        letterHolder->setPosition(gameFrameWidth*count/numFieldHolders + margin, gameFrameHeight*(5.0/7.0));
        letterHolder->setAnchorPoint(Vec2(0.5, 0.5));
        letterHolder->setScale(1.35, 1.35*1.42);
        fieldManager->addChild(letterHolder, 0, i);
    }


}

void Game::update(float delta)
{
    switch (currentPhase)
    {
        case stWord:
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
        break;
        case stKill:        
            spawnFrequencyTimer -= delta;
            if (spawnFrequencyTimer <= 0.0 and unitsUnspawned > 0)
            {
                spawnEnemy();
                spawnFrequencyTimer = 1.0;
            }
            else if (unitsUnspawned == 0)
            {
                if (unitsLeft == 0)
                {
                    killPhaseDone();
                }
            }
            for (auto node : unitManager->getChildren())
            {
                UnitNode* tmp = dynamic_cast<UnitNode*>(node);
                if (tmp->justDied())
                {
                    tmp->setJustDied(false);
                    unitsLeft--;
                    std::cout << unitsLeft << " units left\n";
                }
                else if (tmp->reachedGoal())
                {
                    tmp->setVisible(false);
                    tmp->setHealth(-1);
                    tmp->unsetReachedGoal();
                    loseLife();
                    unitsLeft--;
                }
            }
            break;
        default:
            break;
    }
}


void Game::waitPhaseDone()
{
    startButton->setEnabled(false);
    submitButton->setEnabled(true);
    dealTiles();
    doCountdown = true;
    levelTimer = levelTime;
    currentPhase = stWord;
    std::cout << "Wait Phase Done\n";
}

void Game::wordPhaseDone()
{
    doCountdown = false;
    if (longestWord >= 3)
    {
        towerCount++;
        TowerNode* newTower = createTower(towerCount, longestWord);
        towerManager->addChild(newTower);
        currentTower = newTower;
    }
    lettersFrame->setLocalZOrder(0);
    gameFrame->setLocalZOrder(1);
    globalFrame->setLocalZOrder(2);

    submitButton->setEnabled(false);
    doneButton->setEnabled(true);
    longestWord = 0;
    sValidWord = "";
    vWordsUsed.clear();
    currentWord = "---------";
    bIsValidWord = false;
    bTowerUsed = false;
    clearTiles();
    currentPhase = stBuild;
    std::cout << "Word Phase Done\n";

}

void Game::buildPhaseDone()
{
    if (!bTowerUsed)
    {
        currentTower->removeFromParentAndCleanup(true);
        currentTower = nullptr;
    }
    bTowerPickedUp = false;
    doneButton->setEnabled(false);
    unitsUnspawned = enemiesPerLevel;
    unitsLeft = enemiesPerLevel;
    currentPhase = stKill;
    std::cout << "Build Phase Done\n";
}

void Game::killPhaseDone()
{
    unitTagCounter = 0;
    clearUnits();
    lettersFrame->setLocalZOrder(2);
    globalFrame->setLocalZOrder(1);
    gameFrame->setLocalZOrder(0);
    startButton->setEnabled(true);
    currentLevel++;
    levelLabel->setString("Level " + std::to_string(currentLevel));
    currentPhase = stWait;
    std::cout << "Kill Phase Done\n";

}

void Game::spawnEnemy()
{
    unitTagCounter++;
    std::cout << "spawning enemy " << unitTagCounter << "\n";
    UnitNode* newUnit = createUnit(unitTagCounter, currentLevel);
    unitManager->addChild(newUnit);
    auto action1 = MoveTo::create(3, gridManager->getChildByTag(2)->getPosition());
    auto action2 = MoveTo::create(5, gridManager->getChildByTag(3)->getPosition());
    auto action3 = MoveTo::create(3, gridManager->getChildByTag(4)->getPosition());
    auto action4 = MoveTo::create(3, gridManager->getChildByTag(5)->getPosition());
    auto action5 = MoveTo::create(3, gridManager->getChildByTag(6)->getPosition());
    auto action6 = MoveTo::create(3, gridManager->getChildByTag(7)->getPosition());
    auto action7 = MoveTo::create(3, gridManager->getChildByTag(8)->getPosition());
    auto action8 = MoveTo::create(5, gridManager->getChildByTag(9)->getPosition());
    auto action9 = MoveTo::create(3, gridManager->getChildByTag(10)->getPosition());
    auto action10 = CallFunc::create(CC_CALLBACK_0(UnitNode::setReachedGoal, newUnit));
    auto sequence = Sequence::create(action1, action2, action3, action4, action5, action6, 
        action7, action8, action9, action10, nullptr);
    newUnit->runAction(sequence);
    unitsUnspawned--;
}

TowerNode* Game::createTower(int tag, int level)
{
    std::stringstream ss;
    ss << "Tower" << level << "Idle.png";
    TowerNode* newTower = TowerNode::createTower(ss.str());
    newTower->setLevel(level);
    newTower->applyLevel();
    newTower->setPosition(infoFrame->convertToWorldSpace(loadingZone->getPosition()));
    newTower->setAnchorPoint(Vec2(0,0));
    newTower->setTag(tag);
    newTower->setScale(scale);
    newTower->setName("Tower");
    auto range = PhysicsBody::createCircle(200.0);
    range->setContactTestBitmask(0x01);
    range->setCategoryBitmask(0x01);
    range->setCollisionBitmask(0x01);
    range->setDynamic(false);

    newTower->setPhysicsBody(range);
    return newTower;

}

UnitNode* Game::createUnit(int tag, int level)
{
    UnitNode* newUnit = UnitNode::createUnit("BaseUnit.png");
    newUnit->setHealth(level*2);
    newUnit->setPosition(gridManager->getChildByTag(1)->getPosition());
    newUnit->setAnchorPoint(Vec2(0,0));
    newUnit->setTag(tag);
    newUnit->setScale(scale);
    newUnit->setName("Unit");
    auto pBox = PhysicsBody::createBox(newUnit->getContentSize());
    pBox->setCategoryBitmask(0x01);
    pBox->setCollisionBitmask(0x01);
    pBox->setContactTestBitmask(0x01);
    pBox->setDynamic(false);

    newUnit->setPhysicsBody(pBox);
    return newUnit;
}

void Game::dealTiles()
{
    std::string randomLetters = WordUtils::generateRandomLetters(numLetters);
    for (int i = 0; i<numLetters; ++i)
    {
        std::string filename = "";
        filename += randomLetters.at(i);
        filename += ".png";
        std::cout << filename << "\n";
        auto sprite = LetterNode::createLetter(filename);
        auto holder = getHolderNodeByTag(handManager, i);
        holder->setValue(true);
        sprite->setValue(randomLetters.at(i));
        sprite->setPosition(holder->getPosition());
        sprite->setAnchorPoint(Vec2(0.5, 0.5));
        sprite->setScale(1.35, 1.35);
        letterManager->addChild(sprite, 0, i);
        holder->setLetterTag(sprite->getTag());

    }
}

void Game::updateCurrentWord()
{

    currentWord = "------------------";
    for (auto letter : letterManager->getChildren())
    {
        LetterNode* tmp = dynamic_cast<LetterNode*>(letter);
        if (tmp->inField())
        {
            currentWord[tmp->holderTag()] = tmp->value();
        }
    }
    std::cout << currentWord << "\n";
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
    currentWordLength = formattedWord.length();
    std::cout << "currentWordLength" << currentWordLength << "\n";
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

    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(Game::onContactBegin, this);
    contactListener->onContactSeparate = CC_CALLBACK_1(Game::onContactSeparate, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);

}

bool Game::onContactBegin(PhysicsContact& contact)
{
    if (currentPhase == stKill)
    {
        auto nodeA = contact.getShapeA()->getBody()->getNode();
        auto nodeB = contact.getShapeB()->getBody()->getNode();
        if (nodeA && nodeB)
        {
            if (nodeA->getName() == "Tower")
            {
                if (nodeB->getName() == "Unit")
                {
                    TowerNode* tmpA = dynamic_cast<TowerNode*>(nodeA);
                    UnitNode* tmpB = dynamic_cast<UnitNode*>(nodeB);

                    if (tmpB->health() > 0)
                    {
                        tmpA->addTarget(tmpB);
                        std::cout << "Tower " << tmpA->getTag() << " targetting Unit" << tmpB->getTag() << "\n";
                    }

                }
            }
            else if (nodeB->getName() == "Tower")
            {
                if (nodeA->getName() == "Unit")
                {
                    UnitNode* tmpA = dynamic_cast<UnitNode*>(nodeA);
                    TowerNode* tmpB = dynamic_cast<TowerNode*>(nodeB);
                    if (tmpA->health() > 0)
                    {
                        tmpB->addTarget(tmpA);
                        std::cout << "Tower " << tmpB->getTag() << " targetting Unit" << tmpA->getTag() << "\n";
                    }
                }
            }
        }
    }
    
    //bodies can collide
    return true;
}

bool Game::onContactSeparate(PhysicsContact& contact)
{
    if (currentPhase == stKill)
    {
        auto nodeA = contact.getShapeA()->getBody()->getNode();
        auto nodeB = contact.getShapeB()->getBody()->getNode();

        if (nodeA && nodeB)
        {
            if (nodeA->getName() == "Tower")
            {
                if (nodeB->getName() == "Unit")
                {
                    TowerNode* tmpA = dynamic_cast<TowerNode*>(nodeA);
                    UnitNode* tmpB = dynamic_cast<UnitNode*>(nodeB);
                    tmpA->removeTarget(tmpB);
                    std::cout << "Tower " << nodeA->getTag() << " out of range for Unit " << nodeB->getTag() << "\n";

                }
            }
            else if (nodeB->getName() == "Tower")
            {
                if (nodeA->getName() == "Unit")
                {
                    TowerNode* tmpB = dynamic_cast<TowerNode*>(nodeB);
                    UnitNode* tmpA = dynamic_cast<UnitNode*>(nodeB);
                    tmpB->removeTarget(tmpA);
                    std::cout << "Tower " << nodeB->getTag() << " out of range for Unit " << nodeA->getTag() << "\n";

                }
            }
        }
    }

    //bodies can collide
    return true;
}

void Game::onSubmit(Ref* sender, ui::Widget::TouchEventType type)
{
    if (type == ui::Widget::TouchEventType::ENDED)
    {
        if (currentPhase == stWord)
        {
            if (bIsValidWord)
            {
                if (std::find(vWordsUsed.begin(), vWordsUsed.end(), sValidWord) == vWordsUsed.end())
                {
                    vWordsUsed.push_back(sValidWord);
                    money = WordUtils::calculateMoney(money, sValidWord.length());
                    moneyLabel->setString("$" + std::to_string(money));
                    if ((int)sValidWord.length() > longestWord)
                        longestWord = (int)sValidWord.length();
                }
            }
        }
    }
}

void Game::onStart(Ref* sender, ui::Widget::TouchEventType type)
{
    if (type == ui::Widget::TouchEventType::ENDED)
    {
        if (currentPhase == stWait)
        {
            waitPhaseDone();
        }
    }
}

void Game::onDone(Ref* sender, ui::Widget::TouchEventType type)
{
    if (type == ui::Widget::TouchEventType::ENDED)
    {
        if (currentPhase == stBuild)
        {
            buildPhaseDone();                
        }
    }
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
                bDoShift = false;
                currentLetter = dynamic_cast<LetterNode*>(letterManager->getChildByTag(spriteTouched));
                int fieldTouched = touchedFieldHolder(currentLocation);
                int handTouched = touchedHandHolder(currentLocation);
                if (fieldTouched > -1)
                {
                    originalLocation = fieldManager->getChildByTag(fieldTouched)->getPosition();
                    currentLetter->setInField(false);
                    getHolderNodeByTag(fieldManager, fieldTouched)->setValue(false);
                }
                else if (handTouched > -1)
                {
                    originalLocation = handManager->getChildByTag(handTouched)->getPosition();
                }
                else
                {
                    std::cout << "something went wrong\n";
                }
                lastTouchLocation = currentLocation;
            }
        }
    }
    else if (currentPhase == stBuild)
    {
        int towerTouched = touchedTower(currentLocation);
        if (towerTouched > -1)
        {
            bTowerPickedUp = true;
            currentTower = dynamic_cast<TowerNode*>(towerManager->getChildByTag(towerTouched));
            originalLocation = currentTower->getPosition();
            std::cout << "Tower Touched\n";
            lastTouchLocation = currentLocation;
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
            bLetterMoved = true;
            Vec2 lastLetterLocation = currentLetter->getPosition();
            lastTouchLocation = touch->getLocation();
            currentLetter->setPosition(lastLetterLocation + touch->getDelta());
            Vec2 currentLetterLocation = currentLetter->getPosition();
            if (not fieldBackground->getBoundingBox().containsPoint(currentLetterLocation))
            {
                if (fieldBackground->getBoundingBox().containsPoint(lastLetterLocation))
                {
                    reverseShiftLetters(currentLetter->getPosition());
                    currentLetter->setInField(false);
                }
                bDoShift = false;
            }
            else
            {
                if (not fieldBackground->getBoundingBox().containsPoint(lastLetterLocation))
                {
                    shiftLetters();
                }
                else
                {
                    int lastHolderM = touchedFieldHolder(lastLetterLocation);
                    int currentHolderM = touchedFieldHolder(currentLetterLocation);
                    if (lastHolderM != -1 and currentHolderM != -1)
                    {
                        shiftOneLetter(currentLetter,  lastHolderM - currentHolderM);
                    }
                }
                bDoShift = true;
            }
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
            if (bLetterMoved)
            {
                int fieldTouched = touchedFieldHolder(lastTouchLocation);
                int handTouched = touchedHandHolder(lastTouchLocation);

                if (fieldTouched > -1)
                {
                    placeLetter(currentLetter);

                }
                else if (handTouched > -1)
                {
                    if (bDoShift)
                    {
                        reverseShiftLetters(touch->getLocation());
                        currentLetter->setInField(false);
                    }

                    HolderNode* holder = getHolderNodeByTag(handManager, handTouched);
                    if (!holder->value())
                    {
                        holder->setValue(true);
                        auto action = MoveTo::create(0.1, holder->getPosition());
                        currentLetter->scheduleAction(action);
                    }
                    else
                    {
                        auto action = MoveTo::create(0.1, originalLocation);
                        currentLetter->scheduleAction(action);
                    }
                }
                else
                {
                    if (bDoShift)
                    {
                        reverseShiftLetters(touch->getLocation());
                        currentLetter->setInField(false);
                    }
                    auto action = MoveTo::create(0.1, originalLocation);
                    currentLetter->scheduleAction(action);
                }
                updateValidWord();
                bLetterMoved = false;
            }
            bLetterPickedUp = false;
        }
    }
    else if (currentPhase == stBuild)
    {
        if (bTowerPickedUp)
        {
            std::cout << lastTouchLocation.x << " " << lastTouchLocation.y << "\n";
            int tileTouched = touchedTile(lastTouchLocation);
            std::cout << "tile " << tileTouched << "\n";

            if (tileTouched > -1)
            {
                auto tile = gridManager->getChildByTag(tileTouched);
                if (tile->getName() == "Grass")
                {
                    Vec2 newLocation = gridManager->convertToWorldSpace(tile->getPosition());
                    auto action = MoveTo::create(0.1, newLocation);
                    currentTower->runAction(action);
                    std::cout << "tile moved to tile " << tileTouched << "\n";
                    bTowerUsed = true;
                }
                else
                {
                    auto action = MoveTo::create(0.1, originalLocation);
                    currentTower->runAction(action);
                }
            }
            else
            {
                auto action = MoveTo::create(0.1, originalLocation);
                currentTower->runAction(action);

            }
            bTowerPickedUp = false;

        }
    }
    return true;
}

void Game::placeLetter(LetterNode* letter)
{
    if (currentWordLength == 0)
    {
        HolderNode* middleHolder = getHolderNodeByTag(fieldManager, numLetters-1);
        auto action = MoveTo::create(0.1, middleHolder->getPosition());
        letter->scheduleAction(action);
        leftHolder = middleHolder;
        rightHolder = middleHolder;
        letter->setHolderTag(middleHolder->getTag());
        middleHolder->setValue(true);
        middleHolder->setLetterTag(letter->getTag());
        letter->setInField(true);
    }
    else
    {
        redoHolders();
        letter->setInField(true);
        if (letter->getPosition().x > rightHolder->getPosition().x)
        {
            if (getHolderNodeByTag(fieldManager, rightHolder->getTag()-1)->value())
            {
                rightHolder = getHolderNodeByTag(fieldManager, rightHolder->getTag()+1);
            }
            auto action = MoveTo::create(0.1, rightHolder->getPosition());
            letter->scheduleAction(action);
            letter->setHolderTag(rightHolder->getTag());
            rightHolder->setValue(true);
            rightHolder->setLetterTag(letter->getTag());
        }
        else if (letter->getPosition().x < leftHolder->getPosition().x) 
        {
            if (getHolderNodeByTag(fieldManager, leftHolder->getTag()+1)->value())
            {
                leftHolder = getHolderNodeByTag(fieldManager, leftHolder->getTag()-1);
            }
            auto action = MoveTo::create(0.1, leftHolder->getPosition());
            letter->scheduleAction(action);
            letter->setHolderTag(leftHolder->getTag());
            leftHolder->setValue(true);
            leftHolder->setLetterTag(letter->getTag());
        }   
        else
        {
            HolderNode* holder = getHolderNodeByTag(fieldManager, touchedFieldHolder(letter->getPosition()));
            HolderNode* closeLeftHolder = getHolderNodeByTag(fieldManager, touchedFieldHolder(letter->getPosition())-1);
            HolderNode* closeRightHolder = getHolderNodeByTag(fieldManager, touchedFieldHolder(letter->getPosition())+1);
            if (closeRightHolder->value())
                holder = closeLeftHolder;
            else if (closeLeftHolder->value())
                holder = closeRightHolder;
            auto action = MoveTo::create(0.1, holder->getPosition());
            letter->scheduleAction(action);
            letter->setHolderTag(holder->getTag());
            holder->setValue(true);
            holder->setLetterTag(letter->getTag());
        }
        redoHolders();
    }
    std::cout << "placeLetter \n";
    updateCurrentWord();    
    printDebug();
    moveAllLetters();
}

void Game::moveAllLetters()
{
    for (auto letter : letterManager->getChildren())
    {
        LetterNode* tmp = dynamic_cast<LetterNode*>(letter);
        if (tmp->inField())
        {
            HolderNode* tmpHolder = getHolderNodeByTag(fieldManager, tmp->holderTag());
            auto action = MoveTo::create(0.1, tmpHolder->getPosition());
            tmp->scheduleAction(action);
        }
    }
}

void Game::shiftOneLetter(LetterNode* letter, int direction)
{
    if (not currentWordLength == 0)
    {
        if (getHolderNodeByTag(fieldManager, touchedFieldHolder(letter->getPosition()))->value())
        {
            for (auto letter : letterManager->getChildren())
            {
                LetterNode* tmp = dynamic_cast<LetterNode*>(letter);
                if (tmp->inField())
                {
                    HolderNode* tmpHolder = getHolderNodeByTag(fieldManager, tmp->holderTag());
                    redoHolders();
                    if (direction < 0 and tmpHolder->getTag() != leftHolder->getTag())
                    {
                        HolderNode* tmpRightHolder = getHolderNodeByTag(fieldManager, tmp->holderTag()-2);
                        if (not tmpRightHolder->value())
                        {
                            tmpHolder->setValue(false);
                            auto action = MoveTo::create(0.1, tmpRightHolder->getPosition());
                            tmp->scheduleAction(action);
                            tmp->setHolderTag(tmpRightHolder->getTag());
                            tmpRightHolder->setValue(true);
                            tmpRightHolder->setLetterTag(tmp->getTag());
                            rightHolder = tmpRightHolder;
                            break;
                        }
                    }
                    if (direction > 0 and tmpHolder->getTag() != rightHolder->getTag())
                    {
                        HolderNode* tmpLeftHolder = getHolderNodeByTag(fieldManager, tmp->holderTag()+2);
                        if (not tmpLeftHolder->value())
                        {
                            tmpHolder->setValue(false);
                            auto action = MoveTo::create(0.1, tmpLeftHolder->getPosition());
                            tmp->scheduleAction(action);
                            tmp->setHolderTag(tmpLeftHolder->getTag());
                            tmpLeftHolder->setValue(true);
                            tmpLeftHolder->setLetterTag(tmp->getTag());
                            leftHolder = tmpLeftHolder;
                            break;
                        }
                    }   
                }
            }
            redoHolders();
            std::cout << "shift One\n";
            updateCurrentWord();
            printDebug();    
        }
    }
}

void Game::redoHolders()
{
    /*for (auto holder : fieldManager->getChildren())
    {
        HolderNode* tmp = dynamic_cast<HolderNode*>(holder);
        for (auto letter : letterManager->getChildren())
        {
            LetterNode* tmpLetter = dynamic_cast<LetterNode*>(letter);
            if (tmpLetter->inField())
            {
                if (tmp->value())
                {
                    if (tmp->getTag() == tmpLetter->holderTag())
                        tmp->setValue(true);
                }
                else
                {
                    if (tmp->getTag() == tmpLetter->holderTag())
                        tmp->setValue(false);
                }

            }
        }
    }*/

    for (auto holder : fieldManager->getChildren())
    {
        HolderNode* tmp = dynamic_cast<HolderNode*>(holder);
        if (tmp->value())
        {
            if (tmp->getPosition().x < leftHolder->getPosition().x)
                leftHolder = tmp;
            if (tmp->getPosition().x > rightHolder->getPosition().x)
                rightHolder = tmp;
        }
    }
}

void Game::shiftLetters()
{
    if ((currentWordLength == 0) or (currentWordLength >= 9))
    {
    }
    else
    {
        for (auto letter : letterManager->getChildren())
        {
            LetterNode* tmp = dynamic_cast<LetterNode*>(letter);
            if (tmp->inField())
            {
                if (tmp->getPosition().x < currentLetter->getPosition().x)
                {
                    HolderNode* tmpHolder = getHolderNodeByTag(fieldManager, tmp->holderTag());
                    if (tmpHolder->getTag() > 0)
                    {
                        tmpHolder->setValue(false);
                        HolderNode* newTmpHolder = getHolderNodeByTag(fieldManager, tmp->holderTag()-1);
                        auto action = MoveTo::create(0.1, newTmpHolder->getPosition());
                        tmp->scheduleAction(action);
                        tmp->setHolderTag(newTmpHolder->getTag());
                        newTmpHolder->setValue(true);
                        newTmpHolder->setLetterTag(tmp->getTag());
                    }
                    else
                    {
                        std::cout << "Letter Tried to go off board left\n";
                    }
                }
                else
                {
                    HolderNode* tmpHolder = getHolderNodeByTag(fieldManager, tmp->holderTag());
                    if (tmpHolder->getTag() < (numLetters*2) - 2)
                    {
                        tmpHolder->setValue(false);
                        HolderNode* newTmpHolder = getHolderNodeByTag(fieldManager, tmp->holderTag()+1);
                        auto action = MoveTo::create(0.1, newTmpHolder->getPosition());
                        tmp->scheduleAction(action);
                        tmp->setHolderTag(newTmpHolder->getTag());
                        newTmpHolder->setValue(true);
                        newTmpHolder->setLetterTag(tmp->getTag());
                    }
                    else
                    {
                        std::cout << "Letter Tried to go off board right\n";
                    }
                }
            }
        }
        redoHolders();
        std::cout << "shift all\n";
        updateCurrentWord();
        printDebug();
    }
}

void Game::reverseShiftLetters(Vec2 lastLocation)
{
    if (currentWordLength > 0)
    {
        for (auto letter : letterManager->getChildren())
        {
            LetterNode* tmp = dynamic_cast<LetterNode*>(letter);
            if (tmp->inField())
            {
                if (tmp->getPosition().x < lastLocation.x)
                {
                    auto tmpHolder = getHolderNodeByTag(fieldManager,touchedFieldHolder(tmp->getPosition())+1);
                    auto oldTmpHolder = getHolderNodeByTag(fieldManager,touchedFieldHolder(tmp->getPosition()));
                    auto action = MoveTo::create(0.1, tmpHolder->getPosition());
                    tmp->scheduleAction(action);
                    tmp->setHolderTag(tmpHolder->getTag());
                    oldTmpHolder->setValue(false);
                    tmpHolder->setValue(true);
                    tmpHolder->setLetterTag(tmp->getTag());
                }
                else
                {
                    auto tmpHolder = getHolderNodeByTag(fieldManager,touchedFieldHolder(tmp->getPosition())-1);
                    auto oldTmpHolder = getHolderNodeByTag(fieldManager,touchedFieldHolder(tmp->getPosition()));
                    auto action = MoveTo::create(0.1, tmpHolder->getPosition());
                    leftHolder->setValue(false);
                    tmp->scheduleAction(action);
                    tmp->setHolderTag(tmpHolder->getTag());
                    oldTmpHolder->setValue(false);
                    tmpHolder->setValue(true);
                    tmpHolder->setLetterTag(tmp->getTag());
                }   
            }
        }
        redoHolders();
        std::cout << "reverse shift\n";
        updateCurrentWord();
        printDebug();
    }
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
    for (auto node : gridManager->getChildren())
    {
        if (node->getBoundingBox().containsPoint(location))
            return node->getTag();
    }
    return -1;
}