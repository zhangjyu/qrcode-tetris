#ifndef __CLIENT_LAYER_H__
#define __CLIENT_LAYER_H__

#include "socket/SocketClient.h"
#include "MScen.h"
#include "win.h"
#include "lose.h"
#include "socket/SocketServer.h"

#include "TetrisLayer.h"
class ClientLayer : public Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(ClientLayer);
	void onEnter();
	//void onExit();

	void onRecv(const char* data, int count);
	void onDisconnect();
	MScen* mscen;
	SocketClient* _client;
	SocketServer* _server;
	int stauts = 0;
	void update(float dt);
	TetrisLayer* tlayer;
	void sendend();
	void sendline();
	//void updateOnce(float dt);
	//void getLocalIP(char* ip);
	void aaa(TextFieldTTF *tf);
	const char* s;
	int start = 0;
	const char* ccc;
	void link(Ref* pSender);
	int fll = 0;
	void cre();
	int yc = 0;
protected:
	void initNetwork();
	std::vector<Sprite*> _enemies;
	Label* _lblInfo;
	Sprite* _enemy;
	Animate *_runAnim, *_standAnim;
	Win* win;
	Lose* lo;
};

#endif 
