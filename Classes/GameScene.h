#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__
#include "LetterNode.h"
#include "HolderNode.h"
#include "WordUtils.h"
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
    void clearHolders();    
    void cleanupAll();
    std::string getWord(){ return sValidWord; }
    bool isValidWord(){ return bIsValidWord; }
    CREATE_FUNC(Game);
    void update(float delta) override;
private:
	float fWindowHeight, fWindowWidth;
	void setupUi();
	void setupEvents();

	// --- UI ---
	// Menus
	Sprite* mainFrame;
	Sprite* menuFrame;
	Sprite* gameOverFrame;

	// globalFrame
	Sprite* globalFrame;

	// gameFrame
	Sprite* gameFrame;


	// lettersFrame
	Sprite* lettersFrame;
	Sprite* letterManager;
	Sprite* fieldManager;
	Sprite* handManager;
	ui::Widget* submitButton;
	ui::Widget* startButton;
	ui::Widget* easyButton;
	ui::Widget* hardButton;
	ui::Widget* backToMenuButton;

	// infoFrame
	Sprite* infoFrame;
	Label* timeLabel;
	Label* levelLabel;

	// helpers
	LetterNode* currentLetter;

	// --- Private Variables ---
	Vec2 lastTouchLocation;
	Vec2 originalLocation;
	bool bLetterPickedUp = false;
	bool bLetterMoved = false;
	std::string currentWord = ""; // lenght of 9
	std::string sValidWord;
	bool bIsValidWord;
	std::vector<std::string> vWordsUsed;
	int longestWord;
	bool doCountdown = false;
	float levelTimer = 1.0;
	int currentLevel = 0;
	int currentWordLength = 0;
	bool doRemove;
	bool bScheduleMoveAllLetters;
	bool doMove = false;
	float doMoveTimer = 0.5;
	bool lastInField = false;

	std::vector<int> vFieldTracker;
	void resetField()
	{
		vFieldTracker.clear();
		for (int i=0; i < 17; i++)
		{
			vFieldTracker.push_back(-1);
		}
	}

	bool onTouchStart(Touch* touch, Event* event);
	bool onTouchMove(Touch* touch, Event* event);
	bool onTouchEnd(Touch* touch, Event* event);
	bool onContactBegin(PhysicsContact& contact);
	bool onContactSeparate(PhysicsContact& contact);
	void updateCurrentWord();
	void updateValidWord();
	enum ePhases{stWait, stWord, stMenu, stLose};
	ePhases currentPhase;
	enum eLettersManagers{eLetterManager, eFieldManager, eHandManager, eSubmitButton};
	int touchedLetter(Vec2 location);
	int touchedTile(Vec2 location);
	int touchedHandHolder(Vec2 location);
	int touchedFieldHolder(Vec2 location);
	void placeLetter(LetterNode* letter);
	void removeLetter(LetterNode* letter);
	void shiftLetters(LetterNode* letter);
	void unshiftLetters();
	void moveAllLetters();
	void loadAllWords();
	void onSubmit(Ref* sender, ui::Widget::TouchEventType type);
	void onStart(Ref* sender, ui::Widget::TouchEventType type);
	void onDone(Ref* sender, ui::Widget::TouchEventType type);
	void onEasy(Ref* sender, ui::Widget::TouchEventType type);
	void onHard(Ref* sender, ui::Widget::TouchEventType type);
	void onBackToMenu(Ref* sender, ui::Widget::TouchEventType type);
	HolderNode* getEmptyHolder();
	LetterNode* getLetterNodeByTag(Node* parent, int tag)
	{
	    return dynamic_cast<LetterNode*>(parent->getChildByTag(tag));
	}
	HolderNode* getHolderNodeByTag(Node* parent, int tag)
	{
	    return dynamic_cast<HolderNode*>(parent->getChildByTag(tag));
	}
	HolderNode* getHolderNodeByLoc(Node* parent, Vec2 loc)
	{
		for (auto node : parent->getChildren())
		{
		    if (node->getBoundingBox().containsPoint(loc))
		    	return dynamic_cast<HolderNode*>(node);
		}
	    return nullptr;
	}
	void wordPhaseDone();
	void waitPhaseDone();
	void menuPhaseDone();
	void loseGame();
	void resetData();

	std::vector<std::string> vWords;

	// Resources
	std::string letterImage = "EmptyLetter.png";
	std::string letterHolderImage = "LetterHolder.png";
	std::string fieldHolderImage = "FieldHolder.png";

	// Parameters
	float spawnFrequencyTimer = 1.0;
	int numLetters = 9;
	int numHolders = numLetters*2 - 1;
	float scale = 1.0;
	float levelTime = 30.0;
	int numCorners = 10;
	int enemiesPerLevel = 8; 
	int currentLife = 8;
	float letterScale = 2.3;

	void printFieldVec()
	{
		std::cout << "vFieldTracker: ";
		for (auto tag : vFieldTracker)
			std::cout << " " << tag;
		std::cout << "\n";
		/*std::cout << "LetterInfo: ";
		for (auto letter : letterManager->getChildren())
		{
			LetterNode* tmp = dynamic_cast<LetterNode*>(letter);
			std::cout << tmp->value() << "-" << tmp->inField() << " ";
		}
		std::cout << "\n";*/
	}

	void printDebug()
	{
		std::vector<int> tagsUsed;
	    for (auto letter : letterManager->getChildren())
	    {
	        LetterNode* tmp = dynamic_cast<LetterNode*>(letter);
	        if (tmp->inField())
	        {
	        	std::cout << "Letter " << tmp->value() << " " << tmp->getTag() << " " << tmp->holderTag() << "\n";

	            for (int tag : tagsUsed)
	            {
	            	if (tag == tmp->holderTag())
	            	{
	            		std::cout << "";

	            		//debugMe();
	            	}
	            }
	            tagsUsed.push_back(tmp->holderTag());

	        }
	    }
	    bool last = false;
	    for (auto holder : fieldManager->getChildren())
	    {
	        HolderNode* tmp = dynamic_cast<HolderNode*>(holder);
	        if (tmp->value())
	        {
	        	LetterNode* _letter = dynamic_cast<LetterNode*>(letterManager->getChildByTag(tmp->letterTag()));
	        	std::cout << _letter->value() << " " << tmp->letterTag() << " " << tmp->getTag() << "\n";
	        	if (last)
	        	{
	        		std::cout << "\n-- Error --\n\n";
	        	}
	        	else
	        	{
	        		last = true;
	        	}
	        }
	        else
	        {
	        	last = false;
	        }
	    }
    	std::cout << "\n";
    	//std::cout << "0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7\n";
    	std::cout << "************************\n";	
	    
	}
};

#endif // __GAME_SCENE_H__
