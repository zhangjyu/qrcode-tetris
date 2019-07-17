#ifndef __SERVER_LAYER_H__
#define __SERVER_LAYER_H__

#include "socket/SocketServer.h"
#include  "MScen.h"
#include "win.h"
#include "lose.h"
#include "TetrisLayer.h"
#include "socket/SocketClient.h"
//typedef std::function<void(std::string)> logCallFunc;
enum DataType
{
	RUN,
	STAND,
	POSITION
};

struct GameData
{
	int dataSize;
	DataType dataType;
	Vec2 position;
};
class ServerLayer : public Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(ServerLayer);
	void onEnter();
	//void onExit();

	void update(float dt);
	void initNetwork();
	
	void onRecv(const char* data, int count);
	void onDisconnect();
	std::function<void(HSocket socket)> onNewConnection;
	SocketServer* _server;
	//SocketServer* _servergb;
	SocketClient* _client;
	int stauts = 0;
	TetrisLayer* tlayer;
	//void getLocalIP(char* ip);
	void sendline();
	//void send_brocast();

	void onlog(std::string str);

	void sock();
	int start = 0;
/*	void setlogCallback(logCallFunc func) {
		_calllogFunc = func;
	}*/
	DrawNode *pQRNode;
protected:
	//void initNetwork();
	//void sendData(DataType type);

	Animate *_runAnim, *_standAnim;
	Sprite* _enemy;
	Label* _lblInfo;

	/*void onRotateShape();
	void onMoveLeftShape();
	void onMoveRightShape();
	void onMoveDownShape();

	void onQuit(Ref* pSender);
	void onGetScore(std::string score);
	void onGameOver();
*/	void sendend();
	//void updateOnce(float dt);
	static ServerLayer* slayer;
private:
	
	cocos2d::Label *_scoreLabel;
	MScen* mscen;
	Win* win;
	Lose* lo;
	//logCallFunc _calllogFunc;
};

#endif 
