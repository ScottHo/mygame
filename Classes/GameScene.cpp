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

    SpriteFrame* lettersSpriteFrame = SpriteFrame::create("Grey.png", Rect(0.0, 0.0, gameFrameWidth, gameFrameHeight));
    lettersFrame = Sprite::create();
    lettersFrame->setSpriteFrame(lettersSpriteFrame);
    lettersFrame->setPosition(0, 0);
    lettersFrame->setAnchorPoint(Vec2(0,0));
    lettersFrame->setName("LettersFrame");
    this->addChild(lettersFrame, 1);

    SpriteFrame* infoSpriteFrame = SpriteFrame::create("Beige.png", Rect(0.0, 0.0, infoFrameWidth, infoFrameHeight));
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

    tileManager = Sprite::create();
    tileManager->setSpriteFrame(lettersSpriteFrame);
    tileManager->setPosition(0, 0);
    tileManager->setAnchorPoint(Vec2(0,0));
    tileManager->setName("TileManager");
    gameFrame->addChild(tileManager);

    int tileCounter = 0;
    bool map[(int)gameRows][(int)gameColumns] =
    {{false, false, false, false, false, false, false, false, false},
     {false, true,  true,  false, true,  true,  true,  true,  false},
     {false, true,  true,  false, true,  true,  true,  true,  false},
     {false, true,  true,  false, false, false, true,  true,  false},
     {false, true,  true,  true,  true,  false, true,  true,  false},
     {false, true,  true,  true,  true,  false, true,  true,  false},
     {false, false, false, false, false, false, false, false, false}};

    int currentRow, currentColumn;
    for (float i = 0; i < gameColumns; ++i)
    {
        for (float j = 0; j < gameRows; ++j)
        {
            Sprite* tile;
            currentRow = 6 - (int)j;
            currentColumn = (int)i;
            if (map[currentRow][currentColumn])
            {
                tile = Sprite::create("Grass.png");
                tile->setName("Grass");
            }
            else
            {
                tile = Sprite::create("Path.png");
                tile->setName("Path");
            }

            tile->setPosition(i*tileSize, j*tileSize);
            tile->setAnchorPoint(Vec2(0,0));
            tile->setScale(scale, scale); // somethines wrong with scaling...
            tileManager->addChild(tile, 0, tileCounter);
            ++tileCounter;
        }
    }

    SpriteFrame* loadingZoneFrame = SpriteFrame::create("White.png", Rect(0.0, 0.0, 100.0, 100.0));
    loadingZone = Sprite::create();
    loadingZone->setSpriteFrame(loadingZoneFrame);
    loadingZone->setPosition(infoFrameWidth*2.0/4.0, infoFrameHeight*(3.0/7.0));
    loadingZone->setAnchorPoint(Vec2(0.5,0.5));
    loadingZone->setName("loadingZone");
    loadingZone->setScale(scale);
    infoFrame->addChild(loadingZone, 0);

    auto doneButton = ui::Button::create("DoneButtonUnclicked.png", "DoneButtonClicked.png", "DoneButtonUnclicked.png");
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
        double count = i + .70;
        auto letterHolder = Sprite::create(letterHolderImage);
        auto holder = new Holder(false);
        letterHolder->setUserObject(holder);
        letterHolder->setPosition(gameFrameWidth*count/12.0, gameFrameHeight*(3.0/7.0));
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
        letterHolder->setPosition(gameFrameWidth*count/12.0, gameFrameHeight*(5.0/7.0));
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
    newTower->setPosition(infoFrame->convertToWorldSpace(loadingZone->getPosition()));
    newTower->setAnchorPoint(Vec2(0,0));
    newTower->setUserObject(t);
    newTower->setName("New");
    newTower->setScale(scale);
    towerManager->addChild(newTower);
    gameFrame->setLocalZOrder(1);
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
        if (loadingZone->getBoundingBox().containsPoint(infoFrame->convertToNodeSpace(currentLocation)))
        {
            bTowerPickedUp = true;
            originalLocation = infoFrame->convertToWorldSpace(loadingZone->getPosition());
            currentTower = towerManager->getChildByName("New");
            lastTouchLocation = currentLocation;
        }
        int spriteTouched = touchedTower(currentLocation);
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

        // CLEAN THIS UP PLEASE
        if (bTowerPickedUp)
        {
            int tileTouched = touchedTile(tileManager->convertToNodeSpace(lastTouchLocation));
            if (tileTouched > -1)
            {
                auto tile = tileManager->getChildByTag(tileTouched);
                if (tile->getName() == "Grass")
                {
                    Tower::setEnabledFromNode(currentTower, true);
                    Vec2 newLocation = tileManager->convertToWorldSpace(tile->getPosition());
                    auto action = MoveTo::create(0.2, newLocation);
                    currentTower->runAction(action);
                    std::cout << "tile moved to tile " << tileTouched << "\n";
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
    for (auto node : tileManager->getChildren())
    {
        if (node->getBoundingBox().containsPoint(location))
            return node->getTag();
    }
    return -1;
}