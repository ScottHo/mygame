#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__
#include "cocos2d.h"
#include <string>
USING_NS_CC;

class Game : public cocos2d::Layer
{
public:
    static Scene* createScene();
    virtual bool init();
    void dealTiles();
    void clearTiles();
    void cleanup();
    CREATE_FUNC(Game);
private:
	float fWindowHeight, fWindowWidth;
	void setupUi();
	void setupEvents();
	Sprite* bottomSprite;
	Sprite* topSprite;
	Sprite* letterManager;
	Sprite* fieldManager;
	Sprite* handManager;
	Node* currentLetter;
	Node* currentHolder;
	Vec2 lastTouchLocation;
	Vec2 originalLocation;
	bool bLetterPickedUp;
	bool onTouchStart(Touch* touch, Event* event);
	bool onTouchMove(Touch* touch, Event* event);
	bool onTouchEnd(Touch* touch, Event* event);

	enum letters{eLetter0, eLetter1, eLetter2, eLetter3, eLetter4,
		eLetter5, eLetter6, eLetter7, eLetter8, eLetter9};
	enum managers{eLetterManager, eFieldManager, eHandManager};
	int touchedLetter(Vec2 location);
	int touchedHandHolder(Vec2 location);
	int touchedFieldHolder(Vec2 location);

	// Resources
	std::string letterImage = "EmptyLetter.png";
	std::string letterHolderImage = "LetterHolder.png";
};

#endif // __GAME_SCENE_H__
