#include "socket/SocketClient.h"
//创建一个SocketClient对象
#include "cocos2d.h"
using namespace cocos2d;
SocketClient* SocketClient::construct()
{
	SocketClient* client = new SocketClient;
	return client;
}
//销毁一个SocketClient对象
void SocketClient::destroy()
{
	delete this;
}
//构造函数
SocketClient::SocketClient(void) :
	onRecv(nullptr),
	_socektClient(0)
{
}
//析构函数
SocketClient::~SocketClient(void)
{
	this->clear();
}
//关闭客户段
void SocketClient::clear()
{
	if (_socektClient != 0)
	{
		_mutex.lock();
		this->closeConnect(_socektClient);
		_mutex.unlock();
	}

	for (auto msg : _UIMessageQueue)
	{
		CC_SAFE_DELETE(msg);
	}
	_UIMessageQueue.clear();

	Director::getInstance()->getScheduler()->unscheduleAllForTarget(this);
}

bool SocketClient::initClient()
{
	this->clear();

	_socektClient = socket(AF_INET, SOCK_STREAM, 0);
	if (error(_socketServer))
	{
		log("init client error!");
		_socektClient = 0;
		return false;
	}
	
	Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);
	return true;
}

bool SocketClient::connectServer(const char* serverIP, unsigned short port)
{
	if (!this->initClient())
	{
		return false;
	}

	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(struct sockaddr_in));
	
	 

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = inet_addr(serverIP);
	int ret = 0;
	ret = connect(_socektClient, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr));
	log("connectserver::%d", ret);
	if (ret < 0)
	{
		_socektClient = 0;
		return false;
	}
	stauts = 1;
	std::thread recvThread(&SocketClient::recvMessage, this);
	recvThread.detach();
	return true;
}

void SocketClient::recvMessage()
{
	char recvBuf[1024];
	int ret = 0;
	while (true)
	{
		ret = recv(_socektClient, recvBuf, sizeof(recvBuf), 0);
		if (ret < 0)
		{
			log("recv error!");
			break;
		}
		if (ret > 0 && onRecv != nullptr)
		{
			std::lock_guard<std::mutex> lk(_UIMessageQueueMutex);
			SocketMessage * msg = new SocketMessage(RECEIVE, (unsigned char*)recvBuf, ret);
			_UIMessageQueue.push_back(msg);
		}
	}
	_mutex.lock();
	this->closeConnect(_socektClient);
	if (onDisconnect != nullptr)
	{
		std::lock_guard<std::mutex> lk(_UIMessageQueueMutex);
		SocketMessage * msg = new SocketMessage(DISCONNECT);
		_UIMessageQueue.push_back(msg);
	}
	_socektClient = 0;
	_mutex.unlock();
}

void SocketClient::sendMessage(const char* data, int count)
{
	if (_socektClient != 0)
	{
		int ret = send(_socektClient, data, count, 0);
		if (ret < 0)
		{
			log("send error!");
		}
	}
}

void SocketClient::update(float dt)
{
	if (_UIMessageQueue.size() == 0)
	{
		return;
	}

	_UIMessageQueueMutex.lock();

	if (_UIMessageQueue.size() == 0)
	{
		_UIMessageQueueMutex.unlock();
		return;
	}

	SocketMessage *msg = *(_UIMessageQueue.begin());
	_UIMessageQueue.pop_front();

	switch (msg->getMsgType())
	{
	case DISCONNECT:
		if (onDisconnect)
		{
			this->onDisconnect();
		}
		break;
	case RECEIVE:
		if (onRecv)
		{
			this->onRecv((const char*)msg->getMsgData()->getBytes(), msg->getMsgData()->getSize());
		}
		break;
	default:
		break;
	}

	CC_SAFE_DELETE(msg);
	_UIMessageQueueMutex.unlock();
}

int SocketClient::recv_brocast() {
	//std::thread th(&SocketClient::sock, this);
	//th.detach();
	//connectServer("10.0.20.117", 8888);
	int s=0;
/*#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo method;
	bool isHave = JniHelper::getStaticMethodInfo(method, "org/coco2dx/cpp/socket", "server", "()Ljava/lang/String;");
	if (!isHave) {
		log("jni:此函数不存在");
	}
	else {
		jstring jstr = (jstring)method.env->CallStaticObjectMethod(method.classID, method.methodID);
		const char* str = method.env->GetStringUTFChars(jstr, NULL);
		method.env->ReleaseStringUTFChars(jstr, str);
		method.env->DeleteLocalRef(jstr);
		connectServer(str, 8888);
		s = str;
	}*/
#if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	sock();
#endif
	//sock();
	
	//return 0;
	return s;
}
void SocketClient::sock() {
	int str_len;
	char buff[30];
	int iIndex = 0;

	int addr_len = sizeof(struct sockaddr_in);


	//初始化receiver_socket
	receiver_socket = socket(AF_INET, SOCK_DGRAM, 0);
	//log("receiver_socket:%d", receiver_socket);
	memset(&receiver_addr, 0, sizeof(receiver_addr));
	receiver_addr.sin_family = AF_INET;
	receiver_addr.sin_addr.s_addr = INADDR_ANY;
	receiver_addr.sin_port = htons(6000);

	
	//绑定地址
	int b = 0;
	b = bind(receiver_socket, (struct sockaddr *)&receiver_addr, sizeof(receiver_addr));
	log("b:%d", b);
	//接受数据recv为单纯接受广播数据
	//	if (b == -1)return 3;
	int addrlen = sizeof(receiver_addr);
	while (1) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		str_len = recvfrom(receiver_socket, buff, sizeof(buff), 0, (struct sockaddr*)&receiver_addr, &(addrlen));
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		str_len = recvfrom(receiver_socket, buff, sizeof(buff), 0, (struct sockaddr*)&receiver_addr, (socklen_t*)&(addrlen));
#endif 

		//		if (str_len == -1)return 2;

		log("ip:%s", (char*)inet_ntoa(receiver_addr.sin_addr));
		if (str_len > 0) {
			int c = 0;
			c = connectServer((char*)inet_ntoa(receiver_addr.sin_addr), 8888);
			log("cccccc:::::%d", c);
			log("lianjieshangle");
			stauts = 1;
			break;
		}
		log("str_len::%d", str_len);
		log("%s", buff);
	}

}