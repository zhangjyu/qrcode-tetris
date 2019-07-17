#ifndef __SOCKET_SERVER_H__
#define __SOCKET_SERVER_H__

#include "SocketBase.h"


struct RecvData
{
	HSocket socketClient;
	int dataLen;
	char data[1024];
};


class SocketServer : public SocketBase
{
public:
	static SocketServer* getInstance();
	void destroyInstance();

	bool startServer(unsigned short port);
	void sendMessage(HSocket socket, const char* data, int count);
	void sendMessage(const char* data, int count);
	void update(float dt);
	
	std::function<void(const char* ip)> onStart;
	std::function<void(HSocket socket)> onNewConnection;
	std::function<void(HSocket socket, const char* data, int count)> onRecv;
	std::function<void(HSocket socket)> onDisconnect;
	int stauts = 0;
	HSocket _socketServer;
	HSocket brd_socker;

	int send_brocast();
	char *ip;
	char *ipp;
	char *ippp[10];
	void sock();
	int android = 100;
	void cl();
	//void ansocket();
CC_CONSTRUCTOR_ACCESS:
	SocketServer();
	~SocketServer();

private:
	void clear();
	bool initServer(unsigned short port);
	void acceptClient();
	void acceptFunc();
	void newClientConnected(HSocket socket);
	void recvMessage(HSocket socket);
private:
	static SocketServer* s_server;
	
	unsigned short _serverPort;
	
private:
	std::list<HSocket> _clientSockets;
	std::list<SocketMessage*> _UIMessageQueue;
	std::mutex   _UIMessageQueueMutex;
	

};
#endif