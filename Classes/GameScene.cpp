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
    towerMenu->clearTower();
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
    globalFrame->addChild(towerManager, 1);

    towerMenu = TowerMenu::createMenu();
    towerMenu->addButtons();
    towerMenu->setAnchorPoint(Vec2(0,0));
    this->addChild(towerMenu, -1);

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
    moneyLabel->setString("$" + std::to_string(towerMenu->money()));
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
    if (towerMenu)
        moneyLabel->setString("$" + std::to_string(towerMenu->money()));
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
            if (bScheduleMoveAllLetters)
            {
                moveAllLetters();
                bScheduleMoveAllLetters = false;
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
    resetField();
    doCountdown = true;
    bScheduleMoveAllLetters = false;
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
    currentWord = "---------------";
    bIsValidWord = false;
    bTowerUsed = false;
    clearTiles();
    resetField();
    currentPhase = stBuild;
    std::cout << "Word Phase Done\n";

}

void Game::buildPhaseDone()
{
    if (!bTowerUsed)
    {
        currentTower->removeFromParentAndCleanup(true);
    }
    currentTower = nullptr;
    towerMenu->setLocalZOrder(-1);
    towerMenu->clearTower();
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

    currentWord = "";
    for (auto letterTag : vFieldTracker)
    {
        if (letterTag > -1)
        {
            auto letterNode = getLetterNodeByTag(letterManager, letterTag);
            currentWord += letterNode->value();
        }
    }
    std::cout << currentWord << "\n";
}

void Game::updateValidWord()
{
    if (currentWord.length() > 2 and 
        std::find(vWords.begin(), vWords.end(), currentWord) != vWords.end())
    {
        sValidWord = currentWord;
        bIsValidWord = true;
        std::cout << sValidWord << "\n";
    }
    else
    {
        sValidWord = "";
        bIsValidWord = false;
    }
    currentWordLength = currentWord.length();
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
                    towerMenu->setMoney(WordUtils::calculateMoney(towerMenu->money(), sValidWord.length()));
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
                currentLetter = dynamic_cast<LetterNode*>(letterManager->getChildByTag(spriteTouched));
                int fieldTouched = touchedFieldHolder(currentLocation);
                int handTouched = touchedHandHolder(currentLocation);
                if (fieldTouched > -1)
                {
                    lastHolder = getHolderNodeByTag(fieldManager, fieldTouched);
                    lastHolder->setValue(false);
                    doRemove = true;
                    
                }
                else if (handTouched > -1)
                {
                    lastHolder = getHolderNodeByTag(handManager, handTouched);
                    lastHolder->setValue(false);
                    doRemove = false;
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

            currentTower = dynamic_cast<TowerNode*>(towerManager->getChildByTag(towerTouched));
            if (not currentTower->active())
            {
                bTowerPickedUp = true;
                originalLocation = currentTower->getPosition();
                std::cout << "Tower Touched\n";
                lastTouchLocation = currentLocation;
            }
            else
            {
                std::cout << "Tower Touched\n";
            }
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
            if (touchedFieldHolder(currentLetterLocation) < 0)
            {
                if (touchedFieldHolder(lastLetterLocation) >= 0)
                {
                    // Letter has exited the field
                    lastHolder = getHolderNodeByLoc(fieldManager, lastLetterLocation);
                    if (doRemove)
                    {
                        removeLetter(currentLetter);
                    }
                    else
                    {
                        unshiftLetters();
                    }
                    bScheduleMoveAllLetters = true;
                    updateCurrentWord(); 
                    updateValidWord();
                }
            }
            else
            {
                if (touchedFieldHolder(lastLetterLocation) < 0)
                {
                    // Letter has entered the field
                    shiftLetters(currentLetter);
                    bScheduleMoveAllLetters = true;
                    updateCurrentWord(); 
                    updateValidWord();
                }
                else
                {
                    // Letter is shifting in the field
                    int lastHolderM = touchedFieldHolder(lastLetterLocation);
                    int currentHolderM = touchedFieldHolder(currentLetterLocation);
                    if (lastHolderM != currentHolderM)
                    {
                        unshiftLetters();
                        shiftLetters(currentLetter);
                        bScheduleMoveAllLetters = true;
                        updateCurrentWord(); 
                    }
                }
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
                    bScheduleMoveAllLetters = true;
                    updateCurrentWord(); 
                    updateValidWord();
                }
                else if (handTouched > -1)
                {
                    HolderNode* holder = getHolderNodeByTag(handManager, handTouched);
                    if (!holder->value())
                    {
                        holder->setValue(true);
                        auto action = MoveTo::create(0.1, holder->getPosition());
                        currentLetter->scheduleAction(action);
                    }
                    else
                    {
                        auto action = MoveTo::create(0.1, lastHolder->getPosition());
                        currentLetter->scheduleAction(action);
                    }
                }
                else
                {
                    auto action = MoveTo::create(0.1, lastHolder->getPosition());
                    currentLetter->scheduleAction(action);
                }
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
                    currentTower->setActive(true);
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
        else
        {
            TowerNode* touchedTower = getTowerNodeByLoc(towerManager, touch->getLocation());
            if (currentTower == touchedTower)
            {
                std::cout << "Opened Menu\n";
                towerMenu->setTower(touchedTower);
                towerMenu->setPosition(touch->getLocation());
                towerMenu->setLocalZOrder(5);
            }
        }
    }
    return true;
}

void Game::placeLetter(LetterNode* letter)
{
    HolderNode* currentHolder = getHolderNodeByLoc(fieldManager, letter->getPosition());
    int currentTag = currentHolder->getTag();
    if (currentWordLength < 9)
    {
        bool rightmost = true;
        bool leftmost = true;
        for (int i=0; i < 17; i++)
        {
            if (vFieldTracker[i] > -1)
            {
                if (i > currentTag)
                {
                    rightmost = false;
                }
                else if (i < currentTag)
                {
                    leftmost = false;
                }
                else
                {
                    std::cout << "*** Something went wrong with placeLetter ***\n";
                }
            }
        }
        if (leftmost and rightmost)
        {
            vFieldTracker[numLetters-1] = letter->getTag();
            std::cout << "middle : " << numLetters-1 << "\n";
        }
        else if (leftmost)
        {
            vFieldTracker[numLetters-1-currentWordLength] = letter->getTag();
            std::cout << "Left : " << numLetters-1-currentWordLength << "\n";

        }
        else if (rightmost)
        {
            vFieldTracker[numLetters-1+currentWordLength] = letter->getTag();
            std::cout << "Right : " << numLetters-1+currentWordLength << "\n";

        }
        else
        {
            vFieldTracker[currentTag] = letter->getTag();
            std::cout << "None : " << currentTag << "\n";

        }
        letter->setInField(true);
    } 
}

void Game::shiftLetters(LetterNode* letter)
{
    HolderNode* currentHolder = getHolderNodeByLoc(fieldManager, letter->getPosition());
    int currentTag = currentHolder->getTag();
    std::vector<int> vBackup(17, -1);
    bool switched = false;
    if (currentWordLength < 9)
    {
        for (int i=0; i < 17; i++)
        {
            if (vFieldTracker[i] > -1)
            {
                if (i > currentTag)
                {
                    if (!switched)
                    {
                        vBackup[i-1] = -2;
                        switched = true;
                    }
                    vBackup[i+1] = vFieldTracker[i];
                }
                else if (i <= currentTag)
                {
                    vBackup[i-1] = vFieldTracker[i];
                }
            }
        }
        vFieldTracker = vBackup;
    }  
}

void Game::unshiftLetters()
{
    std::vector<int> vBackup(17, -1);
    bool emptyFound = false;
    if (currentWordLength < 9 and currentWordLength >= 1)
    {
        for (int i=0; i < 17; i++)
        {
            if (vFieldTracker[i] > -1)
            {
                if (emptyFound)
                {
                    vBackup[i-1] = vFieldTracker[i];
                }
                else
                {
                    vBackup[i+1] = vFieldTracker[i];
                }
            }
            else if (vFieldTracker[i] == -2)
            {
                emptyFound = true;
            }
        }
        vFieldTracker = vBackup;
    }  
}

void Game::moveAllLetters()
{
    printFieldVec();
    for (int count = 0; count < (int)vFieldTracker.size(); count++)
    {
        int num = vFieldTracker[count];
        if (num > 0)
        {
            auto letter = getLetterNodeByTag(letterManager, num);
            auto holder = fieldManager->getChildByTag(count);
            auto action = MoveTo::create(0.1, holder->getPosition());
            letter->scheduleAction(action);
            letter->setHolderTag(count);
        }
    }
}

void Game::removeLetter(LetterNode* letter)
{
    int letterTag = letter->getTag();
    std::vector<int> vBackup(17, -1);
    if (currentWordLength > 0)
    {
        bool found = false;
        for (int i=0; i < 17; i++)
        {
            if (vFieldTracker[i] > -1)
            {
                if (vFieldTracker[i] == letterTag)
                {
                    found = true;
                }
                else
                {
                    if (found)
                        vBackup[i-1] = vFieldTracker[i];
                    else
                        vBackup[i+1] = vFieldTracker[i];
                }
            }
        }
        letter->setInField(false);
        vFieldTracker = vBackup;
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