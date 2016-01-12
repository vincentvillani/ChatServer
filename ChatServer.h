/*
 * ChatServer.h
 *
 *  Created on: 7 Jan 2016
 *      Author: vincent
 */

#ifndef CHATSERVER_H_
#define CHATSERVER_H_

#include "Socket.h"

#include <vector>
#include <condition_variable>
#include <mutex>
#include <functional>
#include <iostream>

//#include "ChatServerFunctions.h"




class ChatServer
{
public:
	ChatServer();
	virtual ~ChatServer();

	void update();

	Socket* listeningSocket;
	std::vector<Socket*> clientSockets;
	std::mutex clientSocketMutex;

	//Connecting clients
	std::vector<Socket*> pendingClientSockets;
	std::mutex pendingClientSocketMutex;
	std::condition_variable pendingClientSocketCV;

	bool pendingConnectionAvailable();

	void transferPendingClientSockets();

	void pollClientSocketsForWrite();
};



void acceptThreadMain(ChatServer* server, Socket* listeningSocket);

#endif /* CHATSERVER_H_ */
