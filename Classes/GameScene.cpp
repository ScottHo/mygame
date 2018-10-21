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
    dealTiles();
    this->scheduleUpdate();
    currentPhase = stWord;
    return true;
}

void Game::clearTiles()
{
    for (auto node : letterManager->getChildren())
    {
        Letter::clearFromNode(node);
    }
    currentLetter = nullptr;
    letterManager->removeAllChildrenWithCleanup(true);
}

void Game::clearUnits()
{
    for (auto node : unitManager->getChildren())
    {
        Unit::clearFromNode(node);
    }
    unitManager->removeAllChildrenWithCleanup(true);
}

void Game::clearTowers()
{
    for (auto node : towerManager->getChildren())
    {
        Letter::clearFromNode(node);
    }
    currentTower = nullptr;
    towerManager->removeAllChildrenWithCleanup(true);
}

void Game::clearHolders()
{
    for (auto node : fieldManager->getChildren())
    {
        Holder::clearFromNode(node);
    }
    for (auto node : handManager->getChildren())
    {
        Holder::clearFromNode(node);
    }
    currentHolder = nullptr;
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
    loadingZone->setPosition(infoFrameWidth*1.0/4.0, infoFrameHeight*(3.0/7.0));
    loadingZone->setAnchorPoint(Vec2(0.0,0.0));
    loadingZone->setName("loadingZone");
    loadingZone->setScale(scale);
    infoFrame->addChild(loadingZone, 0);

    doneButton = ui::Button::create("DoneButtonUnclicked.png", "DoneButtonClicked.png", "DoneButtonUnclicked.png");
    doneButton->setPosition(Vec2(infoFrameWidth*2.0/4.0, infoFrameHeight*(1.0/7.0)));
    doneButton->setAnchorPoint(Vec2(0.5,0.5));
    doneButton->addTouchEventListener(CC_CALLBACK_2(Game::onDone, this));
    infoFrame->addChild(doneButton);

    auto moneyPrefix = Label::createWithSystemFont("$", "Arial", 16);
    moneyPrefix->setPosition(Vec2(infoFrameWidth*3.0/6.0, infoFrameHeight*(5.0/7.0)));
    moneyPrefix->setAnchorPoint(Vec2(0.5, 0.5));
    infoFrame->addChild(moneyPrefix);
    moneyLabel = Label::createWithSystemFont("0", "Arial", 16);
    moneyLabel->setPosition(Vec2(infoFrameWidth*4.0/6.0, infoFrameHeight*(5.0/7.0)));
    moneyLabel->setAnchorPoint(Vec2(0.5, 0.5));
    infoFrame->addChild(moneyLabel);

    auto timePrefix = Label::createWithSystemFont("0:", "Arial", 16);
    timePrefix->setPosition(Vec2(infoFrameWidth*3.0/6.0, infoFrameHeight*6.0/7.0));
    timePrefix->setAnchorPoint(Vec2(0.5, 0.5));
    infoFrame->addChild(timePrefix);
    timeLabel = Label::createWithSystemFont("", "Arial", 16);
    std::stringstream ss;
    ss << std::fixed << std::setprecision(0) << levelTimer;
    timeLabel->setString(ss.str());
    timeLabel->setPosition(Vec2(infoFrameWidth*4.0/6.0, infoFrameHeight*6.0/7.0));
    timeLabel->setAnchorPoint(Vec2(0.5, 0.5));
    infoFrame->addChild(timeLabel);

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

    auto submitButton = ui::Button::create("SubmitButtonUnclicked.png", "SubmitButtonClicked.png", "SubmitButtonUnclicked.png");
    submitButton->setPosition(Vec2(gameFrameWidth*4.0/9.0, gameFrameHeight*(1.0/7.0)));
    submitButton->setAnchorPoint(Vec2(0.0,0.0));
    submitButton->addTouchEventListener(CC_CALLBACK_2(Game::onSubmit, this));
    lettersFrame->addChild(submitButton, 0, eSubmitButton);

    for (int i = 0; i<numLetters; ++i)
    {
        double count = i + 1.0;
        auto letterHolder = Sprite::create(letterHolderImage);
        auto holder = new Holder(false);
        letterHolder->setUserObject(holder);
        letterHolder->setPosition(gameFrameWidth*count/11.0, gameFrameHeight*(3.0/7.0));
        letterHolder->setAnchorPoint(Vec2(0.5, 0.5));
        letterHolder->setScale(1.3, 1.3);
        handManager->addChild(letterHolder, 0, i);
    }

    for (int i = 0; i<numLetters; ++i)
    {
        double count = i + 1.0;
        auto letterHolder = Sprite::create(letterHolderImage);
        auto holder = new Holder(false);
        letterHolder->setUserObject(holder);
        letterHolder->setPosition(gameFrameWidth*count/11.0, gameFrameHeight*(5.0/7.0));
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
    if (currentPhase == stKill)
    {
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
            if (Unit::getHealthFromNode(node) == 0)
            {
                node->setVisible(false);
            }
        }
    }
}


void Game::wordPhaseDone()
{
    doCountdown = false;
    Tower* t = createTower(longestWord);
    towerCount++;
    //TowerNode* newTower = TowerNode::create("BaseTower.png");
    TowerNode* newTower = TowerNode::createTower("BaseTower.png");
    newTower->setPosition(infoFrame->convertToWorldSpace(loadingZone->getPosition()));
    newTower->setAnchorPoint(Vec2(0,0));
    newTower->setUserObject(t);
    newTower->setTag(towerCount);
    newTower->setScale(scale);
    newTower->setName("Tower");
    auto range = PhysicsBody::createCircle(200.0);
    range->setContactTestBitmask(0x01);
    range->setCategoryBitmask(0x01);
    range->setCollisionBitmask(0x01);
    range->setDynamic(false);

    newTower->setPhysicsBody(range);
    towerManager->addChild(newTower);
    gameFrame->setLocalZOrder(1);
    currentPhase = stBuild;

    doneButton->setEnabled(true);
    longestWord = 0;
    sValidWord = "";
    bIsValidWord = false;
    std::cout << "Word Phase Done\n";

}

void Game::buildPhaseDone()
{
    currentPhase = stKill;
    doneButton->setEnabled(false);
    unitsUnspawned = enemiesPerLevel;
    std::cout << "Build Phase Done\n";
}

void Game::killPhaseDone()
{
    currentPhase = stWait;
    unitTagCounter = 0;
    clearUnits();
    std::cout << "Kill Phase Done\n";
}

void Game::spawnEnemy()
{
    Unit* u = createUnit(2);
    unitTagCounter++;
    std::cout << "spawning enemy " << unitTagCounter << "\n";
    Sprite* newUnit = Sprite::create("BaseUnit.png");
    newUnit->setPosition(gridManager->getChildByTag(1)->getPosition());
    newUnit->setAnchorPoint(Vec2(0,0));
    newUnit->setUserObject(u);
    newUnit->setTag(unitTagCounter);
    newUnit->setScale(scale);
    newUnit->setName("Unit");
    auto pBox = PhysicsBody::createBox(newUnit->getContentSize());
    pBox->setCategoryBitmask(0x01);
    pBox->setCollisionBitmask(0x01);
    pBox->setContactTestBitmask(0x01);
    pBox->setDynamic(false);

    newUnit->setPhysicsBody(pBox);

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
    auto sequence = Sequence::create(action1, action2, action3, action4, action5, action6, 
        action7, action8, action9, nullptr);
    newUnit->runAction(sequence);
    unitsUnspawned--;
    unitsLeft++;
}

Tower* Game::createTower(unsigned int level)
{
    Tower* tower = new Tower(1, 1, 1, false);
    return tower;
}

Unit* Game::createUnit(int health)
{
    Unit* unit = new Unit(health);
    return unit;
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

    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(Game::onContactBegin, this);
    contactListener->onContactSeparate = CC_CALLBACK_1(Game::onContactSeparate, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);

}

bool Game::onContactBegin(PhysicsContact& contact)
{
    auto nodeA = contact.getShapeA()->getBody()->getNode();
    auto nodeB = contact.getShapeB()->getBody()->getNode();
    std::cout << nodeA->getName() << nodeB->getName() << "\n";
    if (nodeA && nodeB)
    {
        if (nodeA->getName() == "Tower")
        {
            if (nodeB->getName() == "Unit")
            {
                if (Unit::getHealthFromNode(nodeB) > 0)
                {
                    TowerNode* tmp = dynamic_cast<TowerNode*>(nodeA);
                    tmp->setTarget(nodeB);
                    std::cout << "contact begin\n";

                }

            }
        }
        else if (nodeB->getName() == "Tower")
        {
            if (nodeA->getName() == "Unit")
            {
                if (Unit::getHealthFromNode(nodeA) > 0)
                {
                    TowerNode* tmp = dynamic_cast<TowerNode*>(nodeB);
                    tmp->setTarget(nodeA);
                    std::cout << "contact begin\n";
                }
            }
        }
    }

    //bodies can collide
    return true;
}

bool Game::onContactSeparate(PhysicsContact& contact)
{
    auto nodeA = contact.getShapeA()->getBody()->getNode();
    auto nodeB = contact.getShapeB()->getBody()->getNode();

    if (nodeA && nodeB)
    {
        if (nodeA->getName() == "Tower")
        {
            if (nodeB->getName() == "Unit")
            {
                TowerNode* tmp = dynamic_cast<TowerNode*>(nodeA);;
                tmp->removeTarget();
                std::cout << "contact end\n";

            }
        }
        else if (nodeB->getName() == "Tower")
        {
            if (nodeA->getName() == "Unit")
            {
                TowerNode* tmp = dynamic_cast<TowerNode*>(nodeB);
                tmp->removeTarget();
                std::cout << "contact end\n";

            }
        }
    }

    //bodies can collide
    return true;
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
            buildPhaseDone();
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
        int towerTouched = touchedTower(currentLocation);
        if (towerTouched > -1)
        {
            bTowerPickedUp = true;
            currentTower = towerManager->getChildByTag(towerTouched);
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
            std::cout << lastTouchLocation.x << " " << lastTouchLocation.y << "\n";
            int tileTouched = touchedTile(lastTouchLocation);
            std::cout << "tile " << tileTouched << "\n";

            if (tileTouched > -1)
            {
                auto tile = gridManager->getChildByTag(tileTouched);
                if (tile->getName() == "Grass")
                {
                    Vec2 newLocation = gridManager->convertToWorldSpace(tile->getPosition());
                    auto action = MoveTo::create(0.2, newLocation);
                    currentTower->runAction(action);
                    std::cout << "tile moved to tile " << tileTouched << "\n";
                    Tower::setEnabledFromNode(currentTower, true);
                }
                else
                {
                    auto action = MoveTo::create(0.2, originalLocation);
                    currentTower->runAction(action);
                }
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
    for (auto node : gridManager->getChildren())
    {
        if (node->getBoundingBox().containsPoint(location))
            return node->getTag();
    }
    return -1;
}