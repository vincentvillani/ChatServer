/*
 * MasterMailbox.cpp
 *
 *  Created on: 23 Jan 2016
 *      Author: vincent
 */

#include "MasterMailbox.h"

MasterMailbox::MasterMailbox(ServerData* serverData, AcceptData* acceptData, NetworkData* networkData) : _serverData(serverData), _acceptData(acceptData), _networkData(networkData)
{
}

MasterMailbox::~MasterMailbox()
{
	delete _serverData;
	delete _acceptData;
	delete _networkData;
}




void MasterMailbox::AcceptThreadAddNewConnectedUser(User* user)
{
	//Create the functor
	std::function<void()> functor(std::bind(ServerHandleNewUser, user, _serverData, this));

	//Add the new work item to the queue
	{
		std::lock_guard<std::mutex> serverQueueLock(_serverData->workQueueMutex);
		_serverData->workQueue.push(functor);
	}

	//Let the server know something has been added
	_serverData->workConditionVariable.notify_one();


}


void MasterMailbox::ServerThreadAcceptThreadShutdown()
{
	std::function<void()> functor(std::bind(acceptThreadShutdown, _acceptData));

	{
		std::lock_guard<std::mutex> acceptQueueLock(_acceptData->mutex);
		_acceptData->workQueue.push(functor);
	}

	_acceptData->conditionVariable.notify_one();
}


void MasterMailbox::AcceptThreadConfirmShutdown()
{
	std::function<void()> functor(std::bind(ServerThreadShutdown, _serverData));

	{
		std::lock_guard<std::mutex> serverQueueLock(_serverData->workQueueMutex);
		_serverData->workQueue.push(functor);
	}

	_serverData->workConditionVariable.notify_one();
}



void MasterMailbox::ServerThreadAddSocketToNetworkThread(int socketHandle)
{
	std::function<void()> functor(std::bind(NetworkThreadAddSocketToMap, _networkData, socketHandle));

	{
		std::lock_guard<std::mutex> workQueueLock(_networkData->mutex);
		_networkData->workQueue.push(functor);
	}

	_networkData->conditionVariable.notify_one();
}


void MasterMailbox::NetworkThreadRemoveUserFromServerThread(int socketHandle)
{
	std::function<void()> functor(std::bind(ServerRemoveUser, _serverData, this, socketHandle));

	{
		std::lock_guard<std::mutex> workQueueLock(_serverData->workQueueMutex);
		_serverData->workQueue.push(functor);
	}

	_serverData->workConditionVariable.notify_one();
}


void MasterMailbox::NetworkThreadUserLoginToServerThread(std::string* username, int socketHandle)
{
	std::function<void()> functor(std::bind(ServerHandleUsername, _serverData, this, username, socketHandle));

	{
		std::lock_guard<std::mutex> workQueueLock(_serverData->workQueueMutex);
		_serverData->workQueue.push(functor);
	}

	_serverData->workConditionVariable.notify_one();
}


void MasterMailbox::NetworkThreadChatMessageToServerThread(std::string username, std::string chatMessage, int socketHandle)
{
	std::function<void()> functor(std::bind(ServerHandleChatMessage, _serverData, this, username, chatMessage, socketHandle));

	{
		std::lock_guard<std::mutex> workQueueLock(_serverData->workQueueMutex);
		_serverData->workQueue.push(functor);
	}

	_serverData->workConditionVariable.notify_one();
}


void MasterMailbox::ServerThreadSendChatMessageToNetworkThread(std::string username, std::string chatMessage, int socketHandle)
{
	std::function<void()> functor = std::function<void()>(std::bind(NetworkThreadStartSendingChatMessage, _networkData, username, chatMessage, socketHandle));

	{
		std::lock_guard<std::mutex> workQueueLock(_networkData->mutex);
		_networkData->workQueue.push(functor);
	}

	_networkData->conditionVariable.notify_one();

}

