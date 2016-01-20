/*
 * AcceptToSeverMailbox.cpp
 *
 *  Created on: 20 Jan 2016
 *      Author: vincent
 */

#include "AcceptToSeverMailbox.h"

#include "ServerThreadFunctions.h"

AcceptToSeverMailbox::AcceptToSeverMailbox(ServerData* serverData)
{
	_serverData = serverData;
}

AcceptToSeverMailbox::~AcceptToSeverMailbox()
{

}


void AcceptToSeverMailbox::AcceptThreadAddNewConnectedUser(User* user)
{
	//Create the functor
	std::function<void()> functor(std::bind(ServerHandleNewUser, _serverData, user));

	//Add the new work item to the queue
	{
		std::lock_guard<std::mutex> serverQueueLock(_serverData->workQueueMutex);
		_serverData->workQueue.push(functor);
	}

	//Let the server know something has been added
	_serverData->workConditionVariable.notify_one();


}


