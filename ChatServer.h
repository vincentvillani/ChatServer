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



#include "User.h"

#include "ActionQueue.h"
#include "Command.h"
#include "LoginCommand.h"

#include "AcceptThreadFunctions.h"
#include "AcceptedUserBuffer.h"


#include "PollArray.h"
#include "NetworkThreadFunctions.h"
#include "NetworkingThreadData.h"



class ChatServer
{

private:

	//CLIENT MANAGEMENT
	//-----------------------------

	//This is a buffer for users that have been accepted, but not yet logged in
	//AcceptedUserBuffer _acceptedUserBuffer;

	//A queue containing actions for the server to complete, when it gets around to it
	ActionQueue _actionQueue;

	//Stores logged in users
	//Key: socket handle
	std::unordered_map<int, User*> _clientUsersMap;


	//Moves a user/users into the _clientUsersMap and passes their socket handle to the network thread so they can be logged in
	void acceptNewUser();

	//-----------------------------


	//NETWORKING THREAD MANAGEMENT
	//-----------------------------

	//Use the
	NetworkingThreadData* _networkingThreadData;


	//-----------------------------





	//Private chat server variables, no threads other than the one server thread should touch these
	Socket* _listeningSocket;






public:
	ChatServer();
	virtual ~ChatServer();

	void update();

};


#endif /* CHATSERVER_H_ */
