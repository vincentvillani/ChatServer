/*
 * ServerToNetworkMailbox.cpp
 *
 *  Created on: 21 Jan 2016
 *      Author: vincent
 */

#include "ServerToNetworkMailbox.h"

#include "NetworkThreadFunctions.h"
#include "ServerThreadFunctions.h"


ServerToNetworkMailbox::ServerToNetworkMailbox(ServerData* serverData, NetworkData* networkData)
{
	_serverData = serverData;
	_networkData = networkData;
}


ServerToNetworkMailbox::~ServerToNetworkMailbox()
{

}


void ServerToNetworkMailbox::ServerAddSocketToNetworkThread(int socketHandle)
{
	std::function<void()> functor(std::bind(NetworkThreadAddSocketToMap, _networkData, socketHandle));

	{
		std::lock_guard<std::mutex> workQueueLock(_networkData->mutex);
		_networkData->workQueue.push(functor);
	}

	_networkData->conditionVariable.notify_one();
}


void ServerToNetworkMailbox::NetworkRemoveUser(int socketHandle)
{
	std::function<void()> functor(std::bind(ServerRemoveUser, _serverData, socketHandle));

	{
		std::lock_guard<std::mutex> workQueueLock(_serverData->workQueueMutex);
		_serverData->workQueue.push(functor);
	}

	_serverData->workConditionVariable.notify_one();
}
