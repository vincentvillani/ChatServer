/*
 * ServerThreadFunctions.cpp
 *
 *  Created on: 18 Jan 2016
 *      Author: vincent
 */

#include "ServerThreadFunctions.h"

#include <unordered_map>
#include <mutex>


void ServerMain(ServerData* serverData)
{

	while(true)
	{
		std::unique_lock<std::mutex> workQueueLock(serverData->workQueueMutex);

		//Wait for something to do...
		serverData->workConditionVariable.wait(workQueueLock, [&] {return serverData->workQueue.size();});

		std::function<void()> workItem = serverData->workQueue.front();
		serverData->workQueue.pop();

		//Release the lock at this point so others or myself can do something with the queue
		workQueueLock.unlock();

		//Do the work
		workItem();
	}
}


void ServerHandleNewUser(ServerData* server, User* user)
{
	//Do things to handle the new user

	//Add the new user
	std::pair<int, User*> newUserPair(user->socket->handle, user);
	server->clientUsersMap.insert(newUserPair);

	printf("Someone connected!\n");

	//Let the network thread know about the new user
	//TODO: IMPLEMENT THIS
}
