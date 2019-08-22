#include "GameScene.h"
#include <iostream>
#include <string>
#include <memory>
#include <algorithm>
#include <random>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "platform/CCFileUtils.h"
#include "base/CCData.h"

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
    setupUi();
    setupEvents();
    this->scheduleUpdate();
    loadAllWords();
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

void Game::clearHolders()
{
    fieldManager->removeAllChildrenWithCleanup(true);
    handManager->removeAllChildrenWithCleanup(true);
}   

void Game::cleanupAll()
{
    clearTiles();
    clearHolders();
}

void Game::setupUi()
{
    // Main frames
    fWindowHeight = Director::getInstance()->getVisibleSize().height;
    fWindowWidth = Director::getInstance()->getVisibleSize().width;

    float lettersFrameHeight = fWindowHeight*9/20;
    float lettersFrameWidth = fWindowWidth;
    float infoFrameHeight = fWindowHeight*1.5/20;
    float infoFrameWidth = fWindowWidth;
    float gameFrameHeight = fWindowHeight - infoFrameHeight - lettersFrameHeight;
    float gameFrameWidth = fWindowWidth;
    std::cout << "infoFrameHeight = " << infoFrameHeight << "\n";
    std::cout << "gameFrameHeight = " << gameFrameHeight << "\n";
    std::cout << "lettersFrameHeight = " << lettersFrameHeight << "\n";

    SpriteFrame* emptyWindowFrame = SpriteFrame::create("Empty.png", 
        Rect(0.0, 0.0, fWindowWidth, fWindowHeight));
    mainFrame = Sprite::create();
    mainFrame->setSpriteFrame(emptyWindowFrame);
    mainFrame->setPosition(0, 0);
    mainFrame->setAnchorPoint(Vec2(0.0, 0.0));
    this->addChild(mainFrame, 1);

    globalFrame = Sprite::create();
    globalFrame->setSpriteFrame(emptyWindowFrame);
    globalFrame->setPosition(0, 0);
    globalFrame->setAnchorPoint(Vec2(0.0, 0.0));
    mainFrame->addChild(globalFrame, 3);

    SpriteFrame* lettersSpriteFrame = SpriteFrame::create("Beige.png", 
        Rect(0.0, 0.0, lettersFrameWidth, lettersFrameHeight));
    SpriteFrame* emptyLettersSpriteFrame = SpriteFrame::create("Empty.png", 
        Rect(0.0, 0.0, lettersFrameWidth, lettersFrameHeight));
    lettersFrame = Sprite::create();
    lettersFrame->setSpriteFrame(lettersSpriteFrame);
    lettersFrame->setPosition(0.0, 0.0);
    lettersFrame->setAnchorPoint(Vec2(0,0));
    lettersFrame->setName("LettersFrame");
    mainFrame->addChild(lettersFrame, 2);

    SpriteFrame* gameSpriteFrame = SpriteFrame::create("Blue.png", 
        Rect(0.0, 0.0, gameFrameWidth, gameFrameHeight));
    gameFrame = Sprite::create();
    gameFrame->setSpriteFrame(gameSpriteFrame);
    gameFrame->setPosition(0.0, lettersFrameHeight);
    gameFrame->setAnchorPoint(Vec2(0,0));
    gameFrame->setName("GameFrame");
    mainFrame->addChild(gameFrame, 1);

    SpriteFrame* infoSpriteFrame = SpriteFrame::create("Grey.png",
        Rect(0.0, 0.0, infoFrameWidth, infoFrameHeight));
    infoFrame = Sprite::create();
    infoFrame->setSpriteFrame(infoSpriteFrame);
    infoFrame->setPosition(0.0, lettersFrameHeight+gameFrameHeight);
    infoFrame->setAnchorPoint(Vec2(0.0, 0.0));
    infoFrame->setName("InfoFrame");
    mainFrame->addChild(infoFrame, 0);


    SpriteFrame* menuWindowFrame = SpriteFrame::create("Blue.png", Rect(0.0, 0.0, fWindowWidth, fWindowHeight));
    menuFrame = Sprite::create();
    menuFrame->setSpriteFrame(menuWindowFrame);
    menuFrame->setPosition(0, 0);
    menuFrame->setAnchorPoint(Vec2(0.0, 0.0));
    this->addChild(menuFrame, 2);

    easyButton = ui::Button::create("EasyButtonUnclicked.png", "EasyButtonClicked.png", "EasyButtonUnclicked.png");
    easyButton->setPosition(Vec2(fWindowWidth*2.0/4.0, fWindowHeight*(4.0/9.0)));
    easyButton->setAnchorPoint(Vec2(0.5, 0.5));
    easyButton->addTouchEventListener(CC_CALLBACK_2(Game::onEasy, this));
    easyButton->setScale(scale, scale);
    menuFrame->addChild(easyButton);

    hardButton = ui::Button::create("HardButtonUnclicked.png", "HardButtonClicked.png", "HardButtonUnclicked.png");
    hardButton->setPosition(Vec2(fWindowWidth*2.0/4.0, fWindowHeight*(2.0/9.0)));
    hardButton->setAnchorPoint(Vec2(0.5, 0.5));
    hardButton->addTouchEventListener(CC_CALLBACK_2(Game::onHard, this));
    hardButton->setScale(scale, scale);
    menuFrame->addChild(hardButton);

    SpriteFrame* loseSpriteFrame = SpriteFrame::create("Empty.png", Rect(0.0, 0.0, fWindowWidth, fWindowHeight));
    gameOverFrame = Sprite::create();
    gameOverFrame->setSpriteFrame(loseSpriteFrame);
    gameOverFrame->setPosition(0, 0);
    gameOverFrame->setAnchorPoint(Vec2(0.0, 0.0));
    this->addChild(gameOverFrame, 1);

    backToMenuButton = ui::Button::create("BackToMenuUnclicked.png", "BackToMenuClicked.png", "BackToMenuUnclicked.png");
    backToMenuButton->setPosition(Vec2(fWindowWidth*2.0/4.0, fWindowHeight*(1.0/9.0)));
    backToMenuButton->setAnchorPoint(Vec2(0.5, 0.5));
    backToMenuButton->addTouchEventListener(CC_CALLBACK_2(Game::onBackToMenu, this));
    backToMenuButton->setScale(scale, scale);
    gameOverFrame->addChild(backToMenuButton);

    timeLabel = Label::createWithSystemFont("0:00", "Arial", 16);
    std::stringstream ss;
    ss << "0:"  << std::setw(2) << std::setfill('0') << std::fixed << std::setprecision(0) << levelTimer;
    timeLabel->setString(ss.str());
    timeLabel->setPosition(Vec2(infoFrameWidth*1.0/6.0, infoFrameHeight*6.0/9.0));
    timeLabel->setAnchorPoint(Vec2(0.5, 0.5));
    timeLabel->setScale(scale, scale);
    infoFrame->addChild(timeLabel);

    levelLabel = Label::createWithSystemFont("0", "Arial", 16);
    levelLabel->setString("Level " + std::to_string(currentLevel));
    levelLabel->setPosition(Vec2(infoFrameWidth*4.0/6.0, infoFrameHeight*7.0/9.0));
    levelLabel->setAnchorPoint(Vec2(0.5, 0.5));
    levelLabel->setScale(scale, scale);
    infoFrame->addChild(levelLabel);

    auto baseFieldHolderSprite = HolderNode::createHolder("FieldHolder.png");
    float fieldHolderWidth = baseFieldHolderSprite->getContentSize().width * letterScale;
    double margin = (lettersFrameWidth - (fieldHolderWidth*(numLetters*2.0-1.0)))/2.0;
    std::cout << margin << " = margin\n";
    std::cout << numLetters*2.0-1.0 << " = numFieldHolders\n";
    std::cout << fieldHolderWidth << " = fieldHolderWidth\n";

    fieldManager = Sprite::create();
    fieldManager->setSpriteFrame(emptyLettersSpriteFrame);
    fieldManager->setPosition(0,0);
    fieldManager->setAnchorPoint(Vec2(0.0,0.0));
    fieldManager->setName("FieldManager");
    lettersFrame->addChild(fieldManager, 0, eFieldManager);

    handManager = Sprite::create();
    handManager->setSpriteFrame(emptyLettersSpriteFrame);
    handManager->setPosition(0,0);
    handManager->setAnchorPoint(Vec2(0.0,0.0));
    handManager->setName("HandManager");
    lettersFrame->addChild(handManager, 0, eHandManager);

    letterManager = Sprite::create();
    letterManager->setSpriteFrame(emptyLettersSpriteFrame);
    letterManager->setPosition(0,0);
    letterManager->setAnchorPoint(Vec2(0.0,0.0));
    letterManager->setName("LetterManager");
    lettersFrame->addChild(letterManager, 0, eLetterManager);

    submitButton = ui::Button::create("SubmitButtonUnclicked.png", "SubmitButtonClicked.png", "SubmitButtonUnclicked.png");
    submitButton->setPosition(Vec2(lettersFrameWidth*2.0/9.0, lettersFrameHeight*(.5/12.0)));
    submitButton->setAnchorPoint(Vec2(0.0,0.0));
    submitButton->addTouchEventListener(CC_CALLBACK_2(Game::onSubmit, this));
    //submitButton->setScale(scale, scale);
    lettersFrame->addChild(submitButton, 0, eSubmitButton);

    startButton = ui::Button::create("StartButtonUnclicked.png", "StartButtonClicked.png", "StartButtonUnclicked.png");
    startButton->setPosition(Vec2(lettersFrameWidth*5.0/9.0, lettersFrameHeight*(.5/12.0)));
    startButton->setAnchorPoint(Vec2(0.0,0.0));
    startButton->addTouchEventListener(CC_CALLBACK_2(Game::onStart, this));
    //startButton->setScale(scale, scale);
    lettersFrame->addChild(startButton, 0, eSubmitButton);

    for (int i = 0; i<numLetters; ++i)
    {
        double count = i + 1.0;
        double heightPosition = 3.5;
        double offset = lettersFrameWidth*1/12;
        if (count >= 5)
        {
            count = count - 4;
            heightPosition = 7;
            offset = 0;
        }
        auto letterHolder = HolderNode::createHolder(letterHolderImage);
        letterHolder->setPosition(lettersFrameWidth*count/6 + offset, lettersFrameHeight*(heightPosition/12.0));
        letterHolder->setAnchorPoint(Vec2(0.5, 0.5));
        letterHolder->setScale(letterScale, letterScale);
        handManager->addChild(letterHolder, 0, i);
    }

    for (int i = 0; i<numLetters*2-1; ++i)
    {
        double count = i + .5;
        auto letterHolder = HolderNode::createHolder("FieldHolder.png");
        letterHolder->setPosition(fieldHolderWidth*count + margin, lettersFrameHeight*(10.0/12.0));
        letterHolder->setAnchorPoint(Vec2(0.5, 0.5));
        letterHolder->setScale(letterScale, letterScale);
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
            if (bScheduleMoveAllLetters)
            {
                doMoveTimer = 0.05;
                bScheduleMoveAllLetters = false;
            }
            else
            {
                doMoveTimer -= delta;
                if (doMoveTimer <= 0.01)
                {
                    moveAllLetters();
                    doMoveTimer = 0.5;
                }
            }
            break;
        default:
            break;
    }
}


void Game::resetData()
{
    currentLevel = 0;
    levelLabel->setString("Level " + std::to_string(currentLevel));
}

void Game::onEasy(Ref* sender, ui::Widget::TouchEventType type)
{
    if (type == ui::Widget::TouchEventType::ENDED)
    {
        menuPhaseDone();
    }
}

void Game::onHard(Ref* sender, ui::Widget::TouchEventType type)
{
    if (type == ui::Widget::TouchEventType::ENDED)
    {
        menuPhaseDone();
    }
}

void Game::onBackToMenu(Ref* sender, ui::Widget::TouchEventType type)
{
    if (type == ui::Widget::TouchEventType::ENDED)
    {
        if (currentPhase == stLose)
        {
            backToMenuButton->setEnabled(false);
            easyButton->setEnabled(true);
            hardButton->setEnabled(true);
            gameOverFrame->setLocalZOrder(0);
            mainFrame->setLocalZOrder(1);
            menuFrame->setLocalZOrder(2);
            currentPhase = stMenu;
        }
    }
}

void Game::menuPhaseDone()
{
    menuFrame->setLocalZOrder(0);
    gameOverFrame->setLocalZOrder(1);
    mainFrame->setLocalZOrder(2);
    easyButton->setEnabled(false);
    hardButton->setEnabled(false);
    startButton->setEnabled(true);
    currentPhase = stWait;

}

void Game::loseGame()
{
    currentPhase = stLose;
    lettersFrame->setLocalZOrder(2);
    globalFrame->setLocalZOrder(1);
    lettersFrame->setLocalZOrder(0);
    gameOverFrame->setLocalZOrder(2);
    backToMenuButton->setEnabled(true);
    resetData();
    std::cout << "You Lost\n";
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
}

void Game::wordPhaseDone()
{
    doCountdown = false;
    lettersFrame->setLocalZOrder(0);
    lettersFrame->setLocalZOrder(1);
    globalFrame->setLocalZOrder(2);
    currentLetter = nullptr;
    submitButton->setEnabled(false);
    longestWord = 0;
    sValidWord = "";
    vWordsUsed.clear();
    currentWord = "";
    bIsValidWord = false;
    clearTiles();
    resetField();
    currentPhase = stWait;

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
        sprite->setScale(letterScale, letterScale);
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
    currentWordLength = currentWord.length();
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
    std::string filestring = FileUtils::getInstance()->getStringFromFile(FileUtils::getInstance()->fullPathForFilename("words.txt"));
    std::istringstream ss(filestring);
    std::string line;
    while (std::getline(ss, line))
    {
        vWords.push_back(line);
    }
    std::cout << "Words: " << vWords.size() << "\n";
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
    /*if (currentPhase == stKill)
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
    }*/
    return true;
}

bool Game::onContactSeparate(PhysicsContact& contact)
{
    /*
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
    }*/
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
                    if ((int)sValidWord.length() > longestWord)
                        longestWord = (int)sValidWord.length();
                    currentLevel = currentLevel + (int)sValidWord.length();
                    levelLabel->setString("Level " + std::to_string(currentLevel));
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
                currentLetter->setInField(false);
                int fieldTouched = touchedFieldHolder(currentLocation);
                int handTouched = touchedHandHolder(currentLocation);
                if (fieldTouched > -1)
                {
                    doRemove = true;
                    lastInField = true;
                }
                else if (handTouched > -1)
                {
                    getHolderNodeByTag(handManager, handTouched)->setValue(false);
                    doRemove = false;
                    lastInField = false;
                }
                else
                {
                    std::cout << "something went wrong\n";
                }
                lastTouchLocation = currentLocation;
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
            Vec2 lastLetterLocation = currentLetter->getPosition();
            lastTouchLocation = touch->getLocation();
            currentLetter->setPosition(lastLetterLocation + touch->getDelta());

            Vec2 currentLetterLocation = currentLetter->getPosition();
            if (touchedFieldHolder(currentLetterLocation) < 0)
            {
                if (lastInField)
                {
                    std::cout << "in Field -> Out of Field\n";
                    if (doRemove)
                    {
                        removeLetter(currentLetter);
                        doRemove = false;
                    }
                    else
                    {
                        unshiftLetters();
                    }
                     if (not bScheduleMoveAllLetters)
                        bScheduleMoveAllLetters = true;
                    updateCurrentWord(); 
                }
                else
                {
                    std::cout << "Out of Field->Out of Field\n";
                }
                lastInField = false;
            }
            else
            {
                if (not lastInField)
                {
                    std::cout << "Out of Field->Into Field\n";
                    shiftLetters(currentLetter);
                    if (not bScheduleMoveAllLetters)
                        bScheduleMoveAllLetters = true;
                    updateCurrentWord(); 
                }
                else
                {
                    std::cout << "In Field -> In Field\n";
                    int lastHolderM = touchedFieldHolder(lastLetterLocation);
                    int currentHolderM = touchedFieldHolder(currentLetterLocation);
                    if (lastHolderM != currentHolderM)
                    {
                        if (doRemove)
                        {
                            removeLetter(currentLetter);
                            doRemove = false;
                        }
                        else
                        {
                            unshiftLetters();
                        }
                        updateCurrentWord();
                        shiftLetters(currentLetter);
                        if (not bScheduleMoveAllLetters)
                            bScheduleMoveAllLetters = true;
                        updateCurrentWord(); 
                    }
                }
                lastInField = true;
            }
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
                std::cout << "placeLetter\n";
                placeLetter(currentLetter);
                bScheduleMoveAllLetters = true;
                updateCurrentWord(); 
                updateValidWord();
            }
            else if (handTouched > -1)
            {
                if (lastInField)
                    unshiftLetters();
                HolderNode* holder = getHolderNodeByTag(handManager, handTouched);
                if (!holder->value())
                {
                    holder->setValue(true);
                    auto action = MoveTo::create(0.1, holder->getPosition());
                    currentLetter->scheduleAction(action);
                }
                else
                {
                    HolderNode* emptyHolder = getEmptyHolder();
                    if (emptyHolder)
                    {
                        auto action = MoveTo::create(0.1, getEmptyHolder()->getPosition());
                        if (currentLetter)
                            currentLetter->scheduleAction(action);
                        emptyHolder->setValue(true);
                    }
                }
            }
            else
            {
                if (lastInField)
                    unshiftLetters();
                HolderNode* emptyHolder = getEmptyHolder();
                if (emptyHolder)
                {
                    auto action = MoveTo::create(0.1, getEmptyHolder()->getPosition());
                    currentLetter->scheduleAction(action);
                    emptyHolder->setValue(true);
                }
            }
            currentLetter = nullptr;
            bLetterPickedUp = false;
        }
    }
    return true;
}

HolderNode* Game::getEmptyHolder()
{
    for (auto tmpNode : handManager->getChildren())
    {
        HolderNode* tmpHolder = dynamic_cast<HolderNode*>(tmpNode);
        if (!tmpHolder->value())
            return tmpHolder;
    }
    return nullptr;
}

void Game::placeLetter(LetterNode* letter)
{
    std::cout << "\nPlace Letter\n";
    std::cout << "--------------------\n";
    std::cout << "--------------------\n";
    printFieldVec();
    HolderNode* currentHolder = getHolderNodeByLoc(fieldManager, letter->getPosition());
    if (!currentHolder)
    {
        HolderNode* emptyHolder = getEmptyHolder();
        if (emptyHolder)
        {
            auto action = MoveTo::create(0.1, getEmptyHolder()->getPosition());
            currentLetter->scheduleAction(action);
            emptyHolder->setValue(true);
        }
        if (lastInField)
            unshiftLetters();
        return;
    }
    int currentTag = currentHolder->getTag();
    int emptySpace = currentHolder->getTag();;
    std::cout << "currentTag = " << currentTag << "\n";
    if (currentWordLength <= numLetters)
    {
        bool rightmost = true;
        bool leftmost = true;
        for (int i=0; i < numHolders; i++)
        {
            if (vFieldTracker[i] > -1)
            {
                if (i >= currentTag)
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
            else if (vFieldTracker[i] == -2)
            {
                emptySpace = i;
            }
        }
        std::cout << "currentWordLength = " << currentWordLength << "\n";
        if (leftmost and rightmost)
        {
            vFieldTracker[numLetters-1] = letter->getTag();
            std::cout << "middle : " << numLetters-1 << "\n";
        }
        else if (leftmost)
        {
            vFieldTracker[numLetters-1-currentWordLength] = letter->getTag();
            std::cout << "Left : " << numLetters-currentWordLength << "\n";

        }
        else if (rightmost)
        {
            vFieldTracker[numLetters-1+currentWordLength] = letter->getTag();
            std::cout << "Right : " << numLetters-1+currentWordLength << "\n";

        }
        else
        {
            vFieldTracker[emptySpace] = letter->getTag();
            std::cout << "None : " << emptySpace << "\n";
        }
        letter->setInField(true);
    } 
    printFieldVec();
    std::cout << "--------------------\n";
    std::cout << "--------------------\n\n";
}

void Game::shiftLetters(LetterNode* letter)
{
    std::cout << "\nShift Letters\n";
    std::cout << "--------------------\n";
    std::cout << "--------------------\n";
    printFieldVec();
    HolderNode* currentHolder = getHolderNodeByLoc(fieldManager, letter->getPosition());
    if (!currentHolder)
        return;
    int currentTag = currentHolder->getTag();
    std::cout << "currentTag = " << currentTag << "\n";
    std::vector<int> vBackup(numHolders, -1);
    bool switched = false;
    if (currentWordLength <= numLetters)
    {
        for (int i=0; i < numHolders; i++)
        {
            if (vFieldTracker[i] > -1)
            {
                if (i == 0)
                {
                    std::cout << "Shifting full letters\n";
                    return;
                }
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
    printFieldVec();
    std::cout << "--------------------\n";
    std::cout << "--------------------\n\n";
}

void Game::unshiftLetters()
{
    
    std::cout << "\nUnshift Letters\n";
    std::cout << "--------------------\n";
    std::cout << "--------------------\n";
    printFieldVec();
    std::vector<int> vBackup(numHolders, -1);
    bool emptyFound = false;
    if (currentWordLength <= numLetters and currentWordLength >= 1)
    {
        for (int i=0; i < numHolders; i++)
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

    printFieldVec();
    std::cout << "--------------------\n";
    std::cout << "--------------------\n\n";
}

void Game::moveAllLetters()
{
    //printFieldVec();
    if (currentLetter)
        std::cout << "Current Letter: " << currentLetter->value() << "\n";
    for (int count = 0; count < (int)vFieldTracker.size(); count++)
    {
        int num = vFieldTracker[count];
        if (num >= 0)
        {
            auto letter = getLetterNodeByTag(letterManager, num);
            auto holder = fieldManager->getChildByTag(count);
            auto action = MoveTo::create(0.1, holder->getPosition());
            if (letter->inField())
            {
                letter->stopAllActions();
                letter->scheduleAction(action);
                letter->setHolderTag(count);
            }
        }
    }
}

void Game::removeLetter(LetterNode* letter)
{
    std::cout << "\nRemove Letter\n";
    std::cout << "--------------------\n";
    std::cout << "--------------------\n";
    printFieldVec();
    int letterTag = letter->getTag();
    std::cout << "letterTag = " << letterTag << "\n";
    std::vector<int> vBackup(numHolders, -1);
    letter->setInField(false);
    if (currentWordLength > 0)
    {
        bool found = false;
        for (int i=0; i < numHolders; i++)
        {
            if (vFieldTracker[i] == letterTag or vFieldTracker[i] == -2)
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
        vFieldTracker = vBackup;
    }   
    printFieldVec();
    std::cout << "--------------------\n";
    std::cout << "--------------------\n\n";
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
