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
	// topFrame
	Sprite* topFrame;
	Sprite* gameView;
	Sprite* tileManager;

	// contextFrame
	Sprite* contextFrame;
	Sprite* lettersView;
	Sprite* letterManager;
	Sprite* fieldManager;
	Sprite* handManager;
	Sprite* buildingView;
	Sprite* loadingZone;

	// infoFrame
	Sprite* infoFrame;
	Sprite* infoView;
	Label* moneyLabel;
	Label* timeLabel;

	Node* currentLetter;
	Node* currentHolder;


	// --- Private Variables ---
	Vec2 lastTouchLocation;
	Vec2 originalLocation;
	bool bLetterPickedUp;
	std::string currentWord = "---------"; // lenght of 9
	std::string sValidWord;
	bool bIsValidWord;
	std::vector<std::string> vWordsUsed;
	unsigned int longestWord;
	int money;
	bool doCountdown = true;
	float levelTimer = 15.0;



	bool onTouchStart(Touch* touch, Event* event);
	bool onTouchMove(Touch* touch, Event* event);
	bool onTouchEnd(Touch* touch, Event* event);
	void updateCurrentWord(char letter, int index);
	void updateValidWord();
	enum gameManagers{eTileManager};
	enum lettersManagers{eLetterManager, eFieldManager, eHandManager, eSubmitButton};
	int touchedLetter(Vec2 location);
	int touchedHandHolder(Vec2 location);
	int touchedFieldHolder(Vec2 location);
	std::string generateRandomLetters();
	void loadAllWords();
	void updateMoney();
	void onSubmit(Ref* sender, ui::Widget::TouchEventType type);
	Tower* createTower(unsigned int level);
	void wordPhaseDone();



	std::vector<std::string> vWords;

	// Resources
	std::string letterImage = "EmptyLetter.png";
	std::string letterHolderImage = "LetterHolder.png";
	int numLetters = 9;
};

#endif // __GAME_SCENE_H__
