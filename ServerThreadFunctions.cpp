/*
 * ServerThreadFunctions.cpp
 *
 *  Created on: 18 Jan 2016
 *      Author: vincent
 */

#include "ServerThreadFunctions.h"

#include <unordered_map>
#include <mutex>

#include "MasterMailbox.h"
#include "Debug.h"


//----------------------------------------------------------------------
//TODO: Change this to two when the networking thread is written
#define OTHER_THREAD_NUM 1

//------------------------------------------------------------------------

void ServerMain(ServerData* serverData, MasterMailbox* masterMailbox)
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

	//printf("All threads shutdown\n");
}


void ServerHandleNewUser(User* user, ServerData* server, MasterMailbox* masterMailbox)
{
	//Do things to handle the new user

	//Add the new user
	std::pair<int, User*> newUserPair(user->socket->handle, user);
	server->clientUsersMap.insert(newUserPair);

	//printf("Someone connected!\n");

	//Let the network thread know about the new user
	masterMailbox->ServerThreadAddSocketToNetworkThread(user->socket->handle);

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

	std::lock_guard<std::mutex> printLock(Debug::printMutex);
	printf("Socket has been closed!\n");

	//TODO: Let all clients know this person has disconnected
}


void ServerHandleUsername(ServerData* server, MasterMailbox* masterMailbox, std::string* username, int socketHandle)
{
	auto iterator = server->clientUsersMap.find(socketHandle);

	if(iterator == server->clientUsersMap.end())
		return;

	User* user = iterator->second;

	if(user->username != NULL)
		delete user->username;

	user->username = username;
	user->hasUsername = true;

	std::lock_guard<std::mutex> printLock(Debug::printMutex);
	printf("%s connected!\n", user->username->c_str());

	//TODO: Let everyone else know this person connected
}


void ServerHandleChatMessage(ServerData* server, MasterMailbox* masterMailbox, std::string username, std::string chatMessage, int socketHandle)
{
	std::lock_guard<std::mutex> printLock(Debug::printMutex);
	printf("%s: %s\n", username.c_str(), chatMessage.c_str());

	//Send the data to everyone that isn't the current user
	for(auto iterator = server->clientUsersMap.begin(); iterator != server->clientUsersMap.end(); ++iterator)
	{
		if(iterator->first != socketHandle)
			masterMailbox->ServerThreadSendChatMessageToNetworkThread(username, chatMessage, iterator->first);

	}
}

void ServerShutdownAllThreads(MasterMailbox* masterMailbox)
{
	masterMailbox->ServerThreadAcceptThreadShutdown();
}

void ServerThreadShutdown(ServerData* server)
{
	server->threadShutdownNum += 1;
}
