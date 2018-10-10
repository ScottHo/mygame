#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__
#include "cocos2d.h"
USING_NS_CC;

class Game : public cocos2d::Layer
{
public:
    static Scene* createScene();
    virtual bool init();
    CREATE_FUNC(Game);
private:
	float fWindowHeight, fWindowWidth;
	void setupUi();
	void setupEvents();
	Sprite* bottomSprite;
	Sprite* mainSprite;
	Sprite* unitManager;
	bool onTouch(Touch* touch, Event* event);
	enum bottomWidgets{eButton0, eButton1, eButton2, eButton3};
	enum topWidgets{eUnitManager, eEnemyBase, eHomeBase};
	int touchedSpriteBottom(Vec2 location);
	void button0Pressed();
	void button1Pressed();
	void button2Pressed();
	void button3Pressed();
};

#endif // __GAME_SCENE_H__
