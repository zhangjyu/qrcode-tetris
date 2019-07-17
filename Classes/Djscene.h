#ifndef _DJSCENE_H
#define _DJSCENE_H
#include "cocos2d.h"
#include<string>
#include "djplay.h"


#include "socket/SocketServer.h"
//typedef std::function<void(void)> endCallFunc;
//typedef std::function<void(void)> checkCallFunc;
class Djscene : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();

	void onRotateShape();
	void onMoveLeftShape();
	void onMoveRightShape();
	void onMoveDownShape();

	/*
	void endCallback(endCallFunc func) {
		_callendFunc = func;
	}
	void checkCallback(checkCallFunc func) {
		_callcheckFunc = func;
	}*/

	void onQuit(Ref* pSender);
	void onGetScore(std::string score);
	void onGameOver();
	void onCheck();
	CREATE_FUNC(Djscene);
	void upline();
	virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event);
	virtual void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event);
private:
	int endX, endY, firstX, firstY;
	Djplay *djplay;
	cocos2d::Label *_scoreLabel;
	//endCallFunc _callendFunc;
	//checkCallFunc _callcheckFunc;
};
#endif // !_MScen_H
