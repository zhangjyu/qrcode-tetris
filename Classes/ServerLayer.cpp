#include "ServerLayer.h"
#include "socket/SocketServer.h"
#include "MScen.h"

#include "socket/SocketClient.h"
#include "QR_Encode.h"
#include "TetrisLayer.h"
#include "win.h"

Scene* ServerLayer::createScene() {
	auto scene = Scene::create();
	auto layer = ServerLayer::create();
	scene->addChild(layer);
	return scene;
}

bool ServerLayer::init() {
	if (!Layer::init()) {
		return false;
	}
	_server = SocketServer::getInstance();
	_server->startServer(8888);
	//ServerLayer::initNetwork();
	

	int l = 5;
	_server->send_brocast();//windows发送广播,android获取ip
	std::string str = StringUtils::format("%d", l);

	auto winSize = Director::getInstance()->getVisibleSize();
	auto lblServer = Label::createWithSystemFont(str, "Arial", 36);
	lblServer->setPosition(Vec2(winSize.width * 0.4f, winSize.height * 0.4f));
	//addChild(lblServer);
	
#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)
	CQR_Encode m_QREncode;
	bool bRet = m_QREncode.EncodeData(0, 0, 1, -1, _server->ipp);
	if (bRet)
	{
		int nSize = 20; // 定义好像素点的大小
		int originalSize = m_QREncode.m_nSymbleSize + (QR_MARGIN * 2);
		pQRNode = DrawNode::create();
		// 绘制像素点
		for (int i = 0; i < m_QREncode.m_nSymbleSize; ++i)
		{
			for (int j = 0; j < m_QREncode.m_nSymbleSize; ++j)
			{
				if (m_QREncode.m_byModuleData[i][j] == 1)
				{
					pQRNode->drawSolidRect(Vec2((i + QR_MARGIN)*nSize, (j + QR_MARGIN)*nSize), Vec2(((i + QR_MARGIN) + 1)*nSize, ((j + QR_MARGIN) + 1)*nSize), Color4F(0, 0, 0, 1));
				}
				else
				{
					pQRNode->drawSolidRect(Vec2((i + QR_MARGIN)*nSize, (j + QR_MARGIN)*nSize), Vec2(((i + QR_MARGIN) + 1)*nSize, ((j + QR_MARGIN) + 1)*nSize), Color4F(1, 1, 1, 1));
				}
			}
		}
		// 绘制外框
		pQRNode->drawSolidRect(Vec2(0, 0), Vec2((m_QREncode.m_nSymbleSize + QR_MARGIN * 2)*nSize, (QR_MARGIN)*nSize), Color4F(1, 1, 1, 1));
		pQRNode->drawSolidRect(Vec2(0, 0), Vec2((QR_MARGIN)*nSize, (m_QREncode.m_nSymbleSize + QR_MARGIN * 2)*nSize), Color4F(1, 1, 1, 1));
		pQRNode->drawSolidRect(Vec2((m_QREncode.m_nSymbleSize + QR_MARGIN)*nSize, 0), Vec2((m_QREncode.m_nSymbleSize + QR_MARGIN * 2)*nSize, (m_QREncode.m_nSymbleSize + QR_MARGIN * 2)*nSize), Color4F(1, 1, 1, 1));
		pQRNode->drawSolidRect(Vec2(0, (m_QREncode.m_nSymbleSize + QR_MARGIN)*nSize), Vec2((m_QREncode.m_nSymbleSize + QR_MARGIN * 2)*nSize, (m_QREncode.m_nSymbleSize + QR_MARGIN * 2)*nSize), Color4F(1, 1, 1, 1));
		Size winSize = Director::getInstance()->getWinSize();
		pQRNode->setPosition(Vec2((winSize.width - nSize*m_QREncode.m_nSymbleSize) / 2, winSize.height - (winSize.height - nSize*m_QREncode.m_nSymbleSize) / 2));
		pQRNode->setScaleY(-1);
		addChild(pQRNode);
	}
	/*auto al = Label::createWithSystemFont(_server->ipp, "Arial", 36);
	al->setPosition(Vec2(winSize.width * 0.4f, winSize.height * 0.4f));
	addChild(al);*/
#endif
	_client = SocketClient::construct();
	_client->onRecv = CC_CALLBACK_2(ServerLayer::onRecv, this);
	_client->onDisconnect = CC_CALLBACK_0(ServerLayer::onDisconnect, this);
	std::thread th(&ServerLayer::sock, this);
	th.detach();
	//sock();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	mscen = MScen::create();
	mscen->endCallback(std::bind(&ServerLayer::sendend, this));
	mscen->checkCallback(std::bind(&ServerLayer::sendline, this));
	addChild(mscen);
#endif
	log("??????????????");
	this->scheduleUpdate();
	
	log("server");
	return true;
}
void ServerLayer::initNetwork() {
	_server = SocketServer::getInstance();
	_server->startServer(8888);
	log("***************************");
	
}
/*
void ServerLayer::sendData(DataType type) {
	GameData data;
	data.dataType = type;
	data.dataSize = sizeof(GameData);
	data.position = _enemy->getPosition();
	int data=4;
	_server->sendMessage((const char*)&data, sizeof(int));
}*/
void ServerLayer::onEnter() {
	Layer::onEnter();
}

void ServerLayer::onDisconnect() {
	_client->~SocketClient();
}
/*void ServerLayer::onExit() {
	//_server->destroyInstance();
	_client->destroy();
	_client = nullptr;
	log("sadaffdafdssafdaf");
	//Director::getInstance()->getEventDispatcher()->removeEventListener(_touchListener);
	//Layer::onExit();
}*/

void ServerLayer::update(float dt) {
	static float curTime = 0;
	curTime += dt;
	if (curTime > dt*2.f) {
		//int data = 4;
		//_server->sendMessage((const char*)&data, sizeof(int));
		curTime -= dt*2.f;
	}
}

void ServerLayer::onRecv(const char* data, int count) {
	int a = 0;
	memcpy(&a, data, count);
	if (a == 404) {
		log("%d", a);
		_server->cl();
		auto wi = win->createScene();
		Director::getInstance()->replaceScene(wi);
	}
	if (a == 1) {
		mscen->upline(1);
	}
	if (a == 2) {
		mscen->upline(2);
	}
	if (a == 3) {
		mscen->upline(3);
	}
	if (a == 4) {
		mscen->upline(4);
	}
	log("%d", a);

	if (a == 56&&start==0) {
		int data = 56;
		_server->sendMessage((const char*)&data, sizeof(int));
		mscen = MScen::create();
		mscen->endCallback(std::bind(&ServerLayer::sendend, this));
		mscen->checkCallback(std::bind(&ServerLayer::sendline, this));
		addChild(mscen);
#if(CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)
		removeChild(pQRNode);
#endif
		start = 1;
	}
}
void ServerLayer::sendend() {
	int data = 404;
	_server->sendMessage((const char*)&data, sizeof(int));
	_server->cl();
	auto scene = lo->createScene();
	Director::getInstance()->replaceScene(scene);
}
void ServerLayer::sendline() {
	int data = mscen->del;
	_server->sendMessage((const char*)&data, sizeof(int));
}
void ServerLayer::sock() {
	while (_server->stauts == 0) {}
	_client->connectServer(_server->ip, 7000);
	//int data = 56;
	//_server->sendMessage((const char*)&data, sizeof(int));
	
}