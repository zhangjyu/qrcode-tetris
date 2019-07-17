#ifndef _MScen_H
#define _MScen_H
#include "cocos2d.h"
#include<string>
#include "TetrisLayer.h"


#include "socket/SocketServer.h"
typedef std::function<void(void)> endCallFunc;
typedef std::function<void(void)> checkCallFunc;
class MScen : public cocos2d::Layer
{
public :
	static cocos2d::Scene* createScene();
	virtual bool init();

	void onRotateShape();
	void onMoveLeftShape();
	void onMoveRightShape();
	void onMoveDownShape();


	void endCallback(endCallFunc func) {
		_callendFunc = func;
	}
	void checkCallback(checkCallFunc func) {
		_callcheckFunc = func;
	}

	void onQuit(Ref* pSender);
	void onGetScore(std::string score);
	void onGameOver();
	void onCheck();
	CREATE_FUNC(MScen);
	void upline(int line);
	virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event);
	virtual void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event);
	int del;
private:
	int endX, endY, firstX, firstY;
	TetrisLayer *_tetrisLayer;
	cocos2d::Label *_scoreLabel;
	endCallFunc _callendFunc;
	checkCallFunc _callcheckFunc;
};
#endif // !_MScen_H
