#include "ClientLayer.h"
#include "MScen.h"
#include "cocos2d.h"

#include "win.h"
#if(CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)
#define JAVA_CLASS  "org/cocos2dx/cpp/AppActivity"
#endif
#if(CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#include "jni.h"
#endif
USING_NS_CC;
Scene* ClientLayer::createScene()
{
	auto scene = Scene::create();
	auto layer = ClientLayer::create();

	scene->addChild(layer);

	return scene;
}

bool ClientLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	_client = SocketClient::construct();
	_client->onRecv = CC_CALLBACK_2(ClientLayer::onRecv, this);
	_client->onDisconnect = CC_CALLBACK_0(ClientLayer::onDisconnect, this);

	_server = SocketServer::getInstance();
	_server->startServer(7000);
	//ClientLayer::initNetwork();
	auto winSize = Director::getInstance()->getVisibleSize();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo info;
	jobject jobj;
	bool b = JniHelper::getStaticMethodInfo(info, JAVA_CLASS, "ccc", "()Ljava/lang/Object;");
	if (b) {
		jobj = info.env->CallStaticObjectMethod(info.classID, info.methodID);
	}
	JniMethodInfo in;
	bool ret = JniHelper::getMethodInfo(in, JAVA_CLASS, "sss", "()V");
	if (ret) {
		in.env->CallVoidMethod(jobj, in.methodID);
	}
	/*TextFieldTTF *tf = TextFieldTTF::textFieldWithPlaceHolder("Please Click Input:", "Arial", 20);
	tf->setPosition(winSize.width / 2 - 50, winSize.height / 2 - 50);
	addChild(tf);
	//监听
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [tf](Touch *t, Event *event) {
		if (tf->getBoundingBox().containsPoint(t->getLocation())) {
			tf->attachWithIME();
		}
		else {
			tf->detachWithIME();
		}
		return false;
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, tf);

	//auto myMenu = Menu::create();
	auto closeItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png", CC_CALLBACK_0(ClientLayer::aaa, this, tf));

	cocos2d::Menu* m = Menu::create(closeItem, NULL);
	m->setPosition(winSize.width / 2 + 100, winSize.height / 2 - 50);
	this->addChild(m);
	*/
	
	

	auto wSize = Director::getInstance()->getVisibleSize();
	auto sprite_back = Sprite::create("queding.png");
	sprite_back->setPosition(wSize.width / 2, wSize.height / 2);	//设置位置
	sprite_back->setAnchorPoint(Point(0.5, 0.5));	//设置锚点
													//根据缩放设置实际大小
	Size size_back = sprite_back->getContentSize();	//获取精灵大小(设置图片的时候，精灵大小为图片大小)
													//根据实际宽度和总宽度，计算期望缩放比率
	float scaleX = (float)winSize.width / (float)size_back.width;
	float scaleY = (float)winSize.height / (float)size_back.height;
	sprite_back->setScale(scaleX, scaleY);	//缩放

	addChild(sprite_back);
	auto closeItem = MenuItemImage::create("qd.png", "qd.png", CC_CALLBACK_1(ClientLayer::link, this));

	auto menu = Menu::create(closeItem, NULL);
	this->addChild(menu);

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	_client->recv_brocast();
	mscen = MScen::create();
	mscen->endCallback(std::bind(&ClientLayer::sendend, this));
	mscen->checkCallback(std::bind(&ClientLayer::sendline, this));
	addChild(mscen);
#endif

	
	
	this->scheduleUpdate();
	
	log("client");
	return true;
}

void ClientLayer::initNetwork()
{
	
	
	int l = 5;
	log("_client->recv");
	l=_client->recv_brocast();
	//std::string str = _client->recv_brocast();
	log("_client->recv");
	std::string str = StringUtils::format("%d", l);
	auto winSize = Director::getInstance()->getVisibleSize();
	auto lblServer = Label::createWithSystemFont(str, "Arial", 36);
	lblServer->setPosition(Vec2(winSize.width * 0.4f, winSize.height * 0.4f));
	addChild(lblServer);
}
	
void ClientLayer::update(float dt) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if (start == 0) {
		int data = 56;
		_server->sendMessage((const char*)&data, sizeof(int));
	}
#endif
	static float curTime = 0;
	curTime += dt;
	if (curTime > dt*2.f) {
		//int a=6;
		//_server->sendMessage((const char*)&a, sizeof(int));
		curTime -= dt*2.f;
	}
}

void ClientLayer::onEnter()
{
	Layer::onEnter();
}

/*void ClientLayer::onExit()
{
	_client->destroy();
	_client = nullptr;
	//_server->destroyInstance();
	//Director::getInstance()->getEventDispatcher()->removeEventListener(_touchListener);
	//Layer::onExit();
}*/



void ClientLayer::onRecv(const char* data, int count)
{
	int a = 0;
	memcpy(&a, data, count);
	if (a == 404) {
		log("%d", a);
		//onExit();
		auto wi = Win::createScene();
		Director::getInstance()->replaceScene(wi);
		
		//_server->onDisconnect;
		//auto scene = Win::createScene();
		//Director::getInstance()->replaceScene(scene);
		//Layer::onExit();
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
	if (a == 56) {
		start = 1;
	}
	log("%d", a);
}

void ClientLayer::onDisconnect()
{
	_client->~SocketClient();
	//_lblInfo->setString("Client disconnect");
}

void ClientLayer::sendend() {
	int data = 404;
	_server->sendMessage((const char*)&data, sizeof(int));
	auto scene = lo->createScene();
	Director::getInstance()->replaceScene(scene);
	//_server->onDisconnect;
	//auto scene = Lose::createScene();
	//Director::getInstance()->replaceScene(scene);
	//Layer::onExit();
}
void ClientLayer::sendline() {
	int data = mscen->del;
	_server->sendMessage((const char*)&data, sizeof(int));
}

/*void ClientLayer::getLocalIP(char* ip) {
	WSADATA wsaData;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != 0) {
		log("0");
	}
	char hostname[256];
	ret = gethostname(hostname, sizeof(hostname));
	if (ret == SOCKET_ERROR) {
		log("1");
	}
	HOSTENT* host = gethostbyname(hostname);
	if (host == NULL) {
		log("2");
	}
	strcpy(ip, inet_ntoa(*(in_addr*)*host->h_addr_list));
	log("%s", ip);
}*/
void ClientLayer::aaa(TextFieldTTF *tf) {
	auto label = Label::createWithTTF("label test", "fonts/Marker Felt.ttf", 32);
	label->setString(tf->getString());
	auto winSize = Director::getInstance()->getVisibleSize();
	label->setPosition(winSize.width / 2 + 200, winSize.height / 2 - 50);
	//addChild(label);
	std::string str = label->getString();
	std::string sss = "172.16.18.4";
	mscen = MScen::create();
	s=str.c_str();
	_client->connectServer(s, 8888);
	mscen->endCallback(std::bind(&ClientLayer::sendend, this));
	mscen->checkCallback(std::bind(&ClientLayer::sendline, this));
	addChild(mscen);
}
void ClientLayer::link(Ref* pSender){
	std::thread th(&ClientLayer::cre, this);
	th.detach();
	if (yc == 0){
		yc = 1;
		while (fll == 0) {}
		int a = 0;
		while (a < 10000) { a++; }
		mscen = MScen::create();
		_client->connectServer(ccc, 8888);
		//MessageBox(ccc, ccc);
		mscen->endCallback(std::bind(&ClientLayer::sendend, this));
		mscen->checkCallback(std::bind(&ClientLayer::sendline, this));
		addChild(mscen);
	}
}

void ClientLayer::cre() {
	
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo inf;
	bool p = JniHelper::getStaticMethodInfo(inf, JAVA_CLASS, "getaaa", "()Ljava/lang/String;");
	if (p) {
		jstring aaa = (jstring)inf.env->CallStaticObjectMethod(inf.classID, inf.methodID);
		std::string sss = JniHelper::jstring2string(aaa);
		ccc = sss.c_str();
	}
	const char* t = "192.168.1.100";
#endif
	
	fll = 1;
}