/*
 * ServerThreadFunctions.cpp
 *
 *  Created on: 18 Jan 2016
 *      Author: vincent
 */

#include "ServerThreadFunctions.h"

#include <unordered_map>
#include <mutex>

//TODO: Change this to two when the networking thread is written
#define OTHER_THREAD_NUM 1

void ServerMain(ServerData* serverData, AcceptToSeverMailbox* mailbox)
{

	while(serverData->threadShutdownNum != OTHER_THREAD_NUM)
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

		//TODO: THIS IS DEBUG REMOVE THIS AFTER TESTING
		//ServerShutdownAllThreads(mailbox);

	}

	printf("All threads shutdown\n");
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


void ServerRemoveUser(ServerData* server, int socketHandle)
{
	auto iterator = server->clientUsersMap.find(socketHandle);

	if(iterator != server->clientUsersMap.end())
	{
		User* userToRemove = iterator->second;
		delete userToRemove;

		server->clientUsersMap.erase(iterator);
	}
}


void ServerShutdownAllThreads(AcceptToSeverMailbox* mailbox)
{
	mailbox->ServerThreadAcceptThreadShutdown();
}

void ServerThreadShutdown(ServerData* server)
{
	server->threadShutdownNum += 1;
}
