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
#include <unordered_map>
#include <condition_variable>
#include <mutex>
#include <functional>
#include <iostream>

#include "PollArray.h"
#include "ThreadPool.h"

#include "ActionQueue.h"
#include "Command.h"
#include "LoginCommand.h"
//#include "ChatServerFunctions.h"




class ChatServer
{

private:

	//Worker pool
	ThreadPool _workerPool;

	ActionQueue _actionQueue;

	//Private chat server variables, no threads other than the one server thread should touch these
	Socket* listeningSocket;
	std::unordered_map<int, Socket*> clientSocketsMap;

	//Polling variables
	PollArray pollArray;


	//Accepting new connections
	bool pendingConnectionAvailable();
	void transferPendingClientSockets();
	void updateAccept();

	//Check which sockets are available for reading
	void pollClientSocketsForRead();


	//Action queue
	void updateActionQueue();
	void processLoginCommand(LoginCommand* loginCommand);


public:
	ChatServer();
	virtual ~ChatServer();

	void update();



	//Connecting clients
	std::vector<Socket*> acceptedSocketsBuffer;
	std::mutex acceptedSocketsBufferMutex;

	//Server Action Queue
	std::queue<Command*> actionQueue;
	std::mutex actionQueueMutex;

};



void acceptThreadMain(ChatServer* server, Socket* listeningSocket);

#endif /* CHATSERVER_H_ */
