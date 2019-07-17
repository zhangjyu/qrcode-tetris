#include "socket/SocketServer.h"
#include "MScen.h"
SocketServer* SocketServer::s_server = nullptr;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include <WinSock2.h>
#pragma comment(lib, "WS2_32.lib")
#define HSocket SOCKET

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <error.h>
#include <arpa/inet.h>		// for inet_**
#include <netdb.h>			// for gethost**
#include <netinet/in.h>		// for sockaddr_in
#include <sys/types.h>		// for socket
#include <sys/socket.h>		// for socket
#include <unistd.h>
#include <stdio.h>		    // for printf
#include <stdlib.h>			// for exit
#include <string.h>			// for bzero
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#define HSocket int
#endif 

SocketServer* SocketServer::getInstance()
{
	if (s_server == nullptr)
	{
		s_server = new SocketServer;
	}

	return s_server;
}//返回一个SocketServer对象

void SocketServer::destroyInstance()
{
	CC_SAFE_DELETE(s_server);
}//销毁SocketServer对象
//构造函数
SocketServer::SocketServer() :
	_socketServer(0),
	onRecv(nullptr),
	onStart(nullptr),
	onNewConnection(nullptr)
{

}
//析构函数
SocketServer::~SocketServer()
{
	this->clear();
};

void SocketServer::clear()
{
	if (_socketServer)
	{
		_mutex.lock();
		this->closeConnect(_socketServer);
		_mutex.unlock();
	}

	for (auto msg : _UIMessageQueue)
	{
		CC_SAFE_DELETE(msg);
	}
	_UIMessageQueue.clear();

	Director::getInstance()->getScheduler()->unscheduleAllForTarget(this);
}

bool SocketServer::startServer(unsigned short port)
{
	if (!initServer(port))
	{
		return false;
	}

	return true;
}

bool SocketServer::initServer(unsigned short port)
{
	if (_socketServer != 0)
	{
		this->closeConnect(_socketServer);
	}
	_socketServer = socket(AF_INET, SOCK_STREAM, 0);
	
	log("%d", _socketServer);
	if (error(_socketServer))
	{
		log("socket error!");
		_socketServer = 0;
		return false;
	}

	do 
	{
		struct sockaddr_in sockAddr;
		memset(&sockAddr, 0, sizeof(sockAddr));
		_serverPort = port;		// save the port

		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port = htons(_serverPort);
		sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		int ret = 0;

		ret = bind(_socketServer, (const sockaddr*)&sockAddr, sizeof(sockAddr));
		if (ret < 0)
		{
			log("bind error!");
			break;
		}
	
		ret = listen(_socketServer, 5);
		if (ret < 0)
		{
			log("listen error!");
			break;
		}
		// start 
		char hostName[256];
		gethostname(hostName, sizeof(hostName));
		struct hostent* hostInfo = gethostbyname(hostName);
		char* ip = inet_ntoa(*(struct in_addr *)*hostInfo->h_addr_list);
		this->acceptClient();

		if (onStart != nullptr)
		{
			log("start server!");
			this->onStart(ip);
		}

		return true;

	} while (false);
	
	this->closeConnect(_socketServer);
	_socketServer = 0;
	return false;
}

void SocketServer::acceptClient()
{
	std::thread th(&SocketServer::acceptFunc, this);
	th.detach();
}

void SocketServer::acceptFunc()
{
	int len = sizeof(sockaddr);
	struct sockaddr_in sockAddr;
	while (true)
	{	
		#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		HSocket clientSock = accept(_socketServer, (sockaddr*)&sockAddr, &len);
		#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		HSocket clientSock = accept(_socketServer, (sockaddr*)&sockAddr, (socklen_t *)&len);
		#endif
		ip = (char*)(inet_ntoa(sockAddr.sin_addr));
		if (error(clientSock))
		{
			log("accept error!");
		}
		else
		{
			stauts = 1;
			this->newClientConnected(clientSock);
			//MScen::createScene();
		}
	}
}

void SocketServer::newClientConnected(HSocket socket)
{
	log("new connect!");
	_clientSockets.push_back(socket);
	std::thread th(&SocketServer::recvMessage, this, socket);
	th.detach();

	if (onNewConnection)
	{
		std::lock_guard<std::mutex> lk(_UIMessageQueueMutex);
		SocketMessage * msg = new SocketMessage(NEW_CONNECTION, (unsigned char*)&socket, sizeof(HSocket));
		_UIMessageQueue.push_back(msg);
	}
}

void SocketServer::recvMessage(HSocket socket)
{
	char buff[1024];
	int ret = 0;

	while (true)
	{
		ret = recv(socket, buff, sizeof(buff), 0);
		if (ret < 0)
		{
			log("recv(%d) error!", socket);
			break;
		}
		else
		{
			if (ret > 0 && onRecv != nullptr)
			{
				std::lock_guard<std::mutex> lk(_UIMessageQueueMutex);
				RecvData recvData;
				recvData.socketClient = socket;
				memcpy(recvData.data, buff, ret);
				recvData.dataLen = ret;
				SocketMessage * msg = new SocketMessage(RECEIVE, (unsigned char*)&recvData, sizeof(RecvData));
				_UIMessageQueue.push_back(msg);
			}
		}
	}

	_mutex.lock();
	this->closeConnect(socket);
	if (onDisconnect != nullptr)
	{
		std::lock_guard<std::mutex> lk(_UIMessageQueueMutex);
		SocketMessage * msg = new SocketMessage(DISCONNECT, (unsigned char*)&socket, sizeof(HSocket));
		_UIMessageQueue.push_back(msg);
	}
	_mutex.unlock();
}

void SocketServer::sendMessage(HSocket socket, const char* data, int count)
{
	for (auto& sock : _clientSockets)
	{
		if (sock == socket)
		{
			int ret = send(socket, data, count, 0);
			if (ret < 0)
			{
				log("send error!");
			}
			break;
		}
	}
}

void SocketServer::sendMessage(const char* data, int count)
{
	for (auto& socket : _clientSockets)
	{
		int ret = send(socket, data, count, 0);
		if (ret < 0)
		{
			log("send error!");
		}
	}
}

void SocketServer::update(float dt)
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
	case NEW_CONNECTION:
		if (onNewConnection)
		{
			this->onNewConnection(*(HSocket*)msg->getMsgData()->getBytes());
		}
		break;
	case DISCONNECT:
		if (onDisconnect)
		{
			this->onDisconnect(*(HSocket*)msg->getMsgData()->getBytes());
		}
		break;
	case RECEIVE:
		if (onRecv)
		{
			RecvData* recvData = (RecvData*)msg->getMsgData()->getBytes();
			this->onRecv(recvData->socketClient, (const char*)recvData->data, recvData->dataLen);
		}
		break;
	default:
		break;
	}

	CC_SAFE_DELETE(msg);
	_UIMessageQueueMutex.unlock();
}
int SocketServer::send_brocast() {
	//std::thread th(&SocketServer::ansocket, this);
	//th.detach();
	//sock();
	int b = 0;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	int i = 0;
	int sockfd;
	struct ifconf ifconf;
	char buf[512];
	struct ifreq *ifreq;
	char* ip;
	ifconf.ifc_len = 512;
	ifconf.ifc_buf = buf;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	ioctl(sockfd, SIOCGIFCONF, &ifconf);
	close(sockfd);
	ifreq = (struct ifreq*)buf;
	ipp = inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr);
	int k = 0;
	for (i = (ifconf.ifc_len / sizeof(struct ifreq)); i > 0; i--) {
		ip = inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr);
		ifreq++;
		k++;
	}
	b = k;
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	SocketServer::sock();
#endif
	
	return b;
}
void SocketServer::sock() {
	int b = 0;
	brd_socker = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	log("%d", brd_socker);
	struct sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(6000);
	sockAddr.sin_addr.s_addr = INADDR_BROADCAST;


	bool bBoardcast = true;
	b = 0;

	b = setsockopt(brd_socker, SOL_SOCKET, SO_BROADCAST, (char*)&bBoardcast, sizeof(bBoardcast));
	
	android = b;
	log("%d", b);
	log("android:%d",android);
	char buf[] = { "Hello, this is boardcast!" };
	int a = 0;
	std::string str = StringUtils::format("%d", b);

	a = sendto(brd_socker, buf, sizeof(buf), 0, (struct sockaddr *)&sockAddr, sizeof(sockAddr));
	log("a::%d", a);
	//if (a == -1)return 2;
	

	char buff[1024];
	while (true)
	{
		a = 0;
		a = sendto(brd_socker, buf, sizeof(buf), 0, (struct sockaddr *)&sockAddr, sizeof(sockAddr));
		//if (a == -1)return 1;
		if (stauts == 1)break;
	}
	//log("ip-----ip------ip%s", ip);

}

/*
void SocketServer::ansocket() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo method;
	bool isHave = JniHelper::getStaticMethodInfo(method, "D:\null\Classes\socket", "send", "()V");
	if (!isHave) {
		log("jni:此函数不存在");
	}
	else {
		method.env->CallStaticVoidMethod(method.classID, method.methodID);
	}
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	sock();
#endif
}*/
void SocketServer::cl() {
	this->closeConnect(_socketServer);
	_socketServer = 0;
}