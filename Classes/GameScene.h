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
	Sprite* bottomSprite;
	Sprite* mainSprite;
	bool onButton0Touch(Touch* touch, Event* event);
	bool onButton1Touch(Touch* touch, Event* event);
	enum bottomWidgets{eButton0, eButton1, eButton2};
};

#endif // __GAME_SCENE_H__
