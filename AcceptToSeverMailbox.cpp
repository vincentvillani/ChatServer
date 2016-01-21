/*
 * AcceptToSeverMailbox.cpp
 *
 *  Created on: 20 Jan 2016
 *      Author: vincent
 */

#include "AcceptToSeverMailbox.h"

#include "ServerThreadFunctions.h"
#include "AcceptThreadMain.h"

AcceptToSeverMailbox::AcceptToSeverMailbox(ServerData* serverData, AcceptData* acceptData)
{
	_serverData = serverData;
	_acceptData = acceptData;
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


void AcceptToSeverMailbox::ServerThreadAcceptThreadShutdown()
{
	std::function<void()> functor(std::bind(acceptThreadShutdown, _acceptData));

	{
		std::lock_guard<std::mutex> acceptQueueLock(_acceptData->mutex);
		_acceptData->workQueue.push(functor);
	}

	_acceptData->conditionVariable.notify_one();
}


void AcceptToSeverMailbox::AcceptThreadConfirmShutdown()
{
	std::function<void()> functor(std::bind(ServerThreadShutdown, _serverData));

	{
		std::lock_guard<std::mutex> serverQueueLock(_serverData->workQueueMutex);
		_serverData->workQueue.push(functor);
	}

	_serverData->workConditionVariable.notify_one();
}


