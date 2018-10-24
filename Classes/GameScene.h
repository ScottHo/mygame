#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__
#include "TowerNode.h"
#include "UnitNode.h"
#include "LetterNode.h"
#include "HolderNode.h"
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include <string>
USING_NS_CC;

class Game : public cocos2d::Layer
{
public:
    static Scene* createScene();
    virtual bool init() override;
    void dealTiles();
    void clearTiles();
    void clearUnits();
    void clearTowers();
    void clearHolders();    
    void cleanup();
    std::string getWord(){ return sValidWord; }
    bool isValidWord(){ return bIsValidWord; }
    void loseLife()
    {
    	lifeLabel->setString(std::to_string(currentLife) + " Lives");
    	currentLife--;
    }
    CREATE_FUNC(Game);
    void update(float delta) override;
private:
	float fWindowHeight, fWindowWidth;
	void setupUi();
	void setupEvents();

	// --- UI ---
	// globalFrame
	Sprite* globalFrame;
	Sprite* towerManager;

	// gameFrame
	Sprite* gameFrame;
	Sprite* gridManager;
	Sprite* unitManager;

	// lettersFrame
	Sprite* lettersFrame;
	Sprite* letterManager;
	Sprite* fieldManager;
	Sprite* handManager;
	ui::Widget* submitButton;
	ui::Widget* startButton;


	// infoFrame
	Sprite* infoFrame;
	Sprite* infoView;
	Label* moneyLabel;
	Label* timeLabel;
	Label* lifeLabel;
	Label* levelLabel;
	Sprite* loadingZone;
	ui::Widget* doneButton;

	// helpers
	LetterNode* currentLetter;
	HolderNode* currentHolder;
	TowerNode* currentTower;


	// --- Private Variables ---
	Vec2 lastTouchLocation;
	Vec2 originalLocation;
	bool bLetterPickedUp;
	bool bTowerPickedUp;
	std::string currentWord = "---------"; // lenght of 9
	std::string sValidWord;
	bool bIsValidWord;
	std::vector<std::string> vWordsUsed;
	int longestWord;
	int money;
	bool doCountdown = false;
	float levelTimer = 1.0;
	int towerCount = 0;
	int unitTagCounter = 0;
	int unitsUnspawned;
	int unitsLeft;
	int currentLevel = 0;
	bool bTowerUsed;

	bool onTouchStart(Touch* touch, Event* event);
	bool onTouchMove(Touch* touch, Event* event);
	bool onTouchEnd(Touch* touch, Event* event);
	bool onContactBegin(PhysicsContact& contact);
	bool onContactSeparate(PhysicsContact& contact);
	void updateCurrentWord(char letter, int index);
	void updateValidWord();
	enum ePhases{stWait, stWord, stBuild, stKill};
	ePhases currentPhase;
	enum eLettersManagers{eLetterManager, eFieldManager, eHandManager, eSubmitButton};
	int touchedLetter(Vec2 location);
	int touchedTile(Vec2 location);
	int touchedTower(Vec2 location);
	int touchedHandHolder(Vec2 location);
	int touchedFieldHolder(Vec2 location);
	std::string generateRandomLetters();
	void loadAllWords();
	void onSubmit(Ref* sender, ui::Widget::TouchEventType type);
	void onStart(Ref* sender, ui::Widget::TouchEventType type);
	void onDone(Ref* sender, ui::Widget::TouchEventType type);
	void spawnEnemy();

	TowerNode* createTower(int tag, int level); 
	UnitNode* createUnit(int tag, int health);
	void wordPhaseDone();
	void buildPhaseDone();
	void killPhaseDone();
	void waitPhaseDone();

	std::vector<std::string> vWords;

	// Resources
	std::string letterImage = "EmptyLetter.png";
	std::string letterHolderImage = "LetterHolder.png";

	// Parameters
	float spawnFrequencyTimer = 1.0;
	int numLetters = 9;
	float scale = 1.0;
	float numRows = 6.0;
	float numColumns = 12.0;
	float gameRows = 6.0;
	float gameColumns = 10.0;
	float levelTime = 10.0;
	int numCorners = 10;
	int enemiesPerLevel = 8;
	int currentLife = 5;
};

#endif // __GAME_SCENE_H__
