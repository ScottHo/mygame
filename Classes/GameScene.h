#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__
#include "Letter.h"
#include "Holder.h"
#include "Unit.h"
#include "Tower.h"
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
    void cleanup();
    std::string getWord(){ return sValidWord; }
    bool isValidWord(){ return bIsValidWord; }
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
	Sprite* tileManager;

	// lettersFrame
	Sprite* lettersFrame;
	Sprite* letterManager;
	Sprite* fieldManager;
	Sprite* handManager;


	// infoFrame
	Sprite* infoFrame;
	Sprite* infoView;
	Label* moneyLabel;
	Label* timeLabel;
	Sprite* loadingZone;

	// helpers
	Node* currentLetter;
	Node* currentHolder;
	Node* currentTower;


	// --- Private Variables ---
	Vec2 lastTouchLocation;
	Vec2 originalLocation;
	bool bLetterPickedUp;
	bool bTowerPickedUp;
	std::string currentWord = "---------"; // lenght of 9
	std::string sValidWord;
	bool bIsValidWord;
	std::vector<std::string> vWordsUsed;
	unsigned int longestWord;
	int money;
	bool doCountdown = true;
	float levelTimer = 5.0;



	bool onTouchStart(Touch* touch, Event* event);
	bool onTouchMove(Touch* touch, Event* event);
	bool onTouchEnd(Touch* touch, Event* event);
	void updateCurrentWord(char letter, int index);
	void updateValidWord();
	enum ePhases{stWait, stWord, stBuild, stKill};
	ePhases currentPhase;
	enum eGameManagers{eTileManager};
	enum eLettersManagers{eLetterManager, eFieldManager, eHandManager, eSubmitButton};
	int touchedLetter(Vec2 location);
	int touchedTile(Vec2 location);
	int touchedTower(Vec2 location);
	int touchedHandHolder(Vec2 location);
	int touchedFieldHolder(Vec2 location);
	std::string generateRandomLetters();
	void loadAllWords();
	void updateMoney();
	void onSubmit(Ref* sender, ui::Widget::TouchEventType type);
	void onDone(Ref* sender, ui::Widget::TouchEventType type);

	Tower* createTower(unsigned int level);
	void wordPhaseDone();
	void buildPhaseDone();



	std::vector<std::string> vWords;

	// Resources
	std::string letterImage = "EmptyLetter.png";
	std::string letterHolderImage = "LetterHolder.png";

	// Parameters
	int numLetters = 9;
	float scale = 1.0;
	float numRows = 7.0;
	float numColumns = 14.0;
	float gameRows = 7.0;
	float gameColumns = 9.0;
};

#endif // __GAME_SCENE_H__
