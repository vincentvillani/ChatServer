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

#include "AcceptedUserBuffer.h"
#include "PollArray.h"
#include "ThreadPool.h"
#include "User.h"

#include "ActionQueue.h"
#include "Command.h"
#include "LoginCommand.h"

#include "AcceptThreadFunctions.h"




class ChatServer
{

private:

	//Worker pool
	ThreadPool _workerPool;

	//A queue containing actions for the server to complete, when it gets around to it
	ActionQueue _actionQueue;

	//This is a buffer for users that have been accepted, but not yet logged in
	AcceptedUserBuffer _acceptedUserBuffer;

	//Private chat server variables, no threads other than the one server thread should touch these
	Socket* _listeningSocket;


	//Key: socket handle
	std::unordered_map<int, User*> _clientUsersMap;

	//Polling variables
	PollArray _pollArray;




	//Accepting new connections
	//bool pendingConnectionAvailable();
	//void transferPendingClientSockets();
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

};


#endif /* CHATSERVER_H_ */
