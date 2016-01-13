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

//#include "ChatServerFunctions.h"




class ChatServer
{

private:

	//Private chat server variables, no threads other than the one server thread should touch these
	Socket* listeningSocket;
	std::unordered_map<int, Socket*> clientSocketsMap;

	//Polling variables
	PollArray pollArray;


	//Accepting new connections
	bool pendingConnectionAvailable();
	void transferPendingClientSockets();

	//Check which sockets are available for reading
	void pollClientSocketsForRead();


	//Update helper methods
	void updateAccept();


public:
	ChatServer();
	virtual ~ChatServer();

	void update();



	//Connecting clients
	std::vector<Socket*> acceptedSocketsBuffer;
	std::mutex acceptedSocketsBufferMutex;

};



void acceptThreadMain(ChatServer* server, Socket* listeningSocket);

#endif /* CHATSERVER_H_ */
