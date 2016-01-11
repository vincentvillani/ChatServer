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

class ChatServer
{
public:
	ChatServer();
	virtual ~ChatServer();

	void update();

	Socket* listeningSocket;
	std::vector<Socket*> clientSockets;
};

#endif /* CHATSERVER_H_ */
