/*
 * ChatServer.h
 *
 *  Created on: 7 Jan 2016
 *      Author: vincent
 */

#ifndef SERVERDATA_H_
#define SERVERDATA_H_

#include <unordered_map>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>


#include "User.h"




class ServerData
{

public:

	//WorkQueue
	std::queue<std::function<void()>> workQueue;
	std::mutex workQueueMutex;
	std::condition_variable workConditionVariable;

	//Socket* listeningSocket; //Socket to listen to new connections
	std::unordered_map<int, User*> clientUsersMap;



	ServerData();
	virtual ~ServerData();

};


#endif /* SERVERDATA_H_ */
