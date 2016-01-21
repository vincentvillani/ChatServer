/*
 * ServerWorkerFunctions.cpp
 *
 *  Created on: 14 Jan 2016
 *      Author: vincentvillani
 */

#include "NetworkThreadFunctions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>

#include "Network.h"
#include "ServerToNetworkMailbox.h"

static void StartAllWork(NetworkData* networkData);
static void RemoveSocketFromMap(NetworkData* networkData, ServerToNetworkMailbox* mailbox, int socketHandle);


//Private functions
//-------------------------------------------
void StartAllWork(NetworkData* networkData)
{

}


void RemoveSocketFromMap(NetworkData* networkData, ServerToNetworkMailbox* mailbox, int socketHandle)
{
	//Remove it from the map on this thread, and call the mailbox to have it be removed on the server thread
	networkData->socketHandleMap.erase(socketHandle);

	mailbox->NetworkRemoveUser(socketHandle);
}

//-------------------------------------------



void NetworkThreadMain(NetworkData* networkData, ServerToNetworkMailbox* mailbox)
{
	while(networkData->shouldContinue)
	{
		std::unique_lock<std::mutex> workQueueLock (networkData->mutex);

		bool workToDo = networkData->conditionVariable.wait_for(workQueueLock, std::chrono::milliseconds(0), [&] {return networkData->workQueue.size();});

		if(workToDo)
		{
			workQueueLock.unlock();

			StartAllWork(networkData);
		}

		//Poll for new connections

		//Read/Write any new data
	}
}


void NetworkThreadAddSocketToMap(NetworkData* network, int socketHandle)
{
	std::pair<int, int> newPair(socketHandle, socketHandle);
	network->socketHandleMap.insert(newPair);
}

/*
void NetworkThreadMain(NetworkingThreadData* networkData)
{
	while(true)
	{
		std::unique_lock<std::mutex> workQueueLock(networkData->mutex);

		//Are there new work items to be done, if not sleep for a bit
		networkData->conditionVar.wait_for(workQueueLock,  std::chrono::milliseconds(50), std::bind(ConditionVariablePredicate, networkData));

		//TODO: This could be more efficent, look into later
		ProcessWorkQueue(networkData);

		//Poll sockets

		//Read/write to sockets asyncly

		//Break and exit on some signal?

	}
}


void ProcessWorkQueue(NetworkingThreadData* networkData)
{
	//If there is nothing to do, just return
	if(!networkData->workQueue.size())
		return;


	//Get the lock and double check if there is anything to do (queue might be in a weird transitional state)
	std::unique_lock<std::mutex> workQueueLock(networkData->mutex);

	if(!networkData->workQueue.size())
		return;


	while(networkData->workQueue.size())
	{
		//Get the next work item and remove it from the queue
		std::function<void()> workItem = networkData->workQueue.front();
		networkData->workQueue.pop();

		//unlock the mutex so we/others can get it
		workQueueLock.unlock();

		//Do the work
		workItem();

		//Get the lock again so we can get get the next item/check things
		workQueueLock.lock();
	}


}


bool ConditionVariablePredicate(NetworkingThreadData* networkData)
{
	return networkData->workQueue.size();
}


void ReadData(ActionQueue* serverActionQueue, Socket* clientSocket)
{

	printf("Reading data...\n");

	//The amount of data that has currently been read from the socket
	int32_t currentlyReadBytes = 0;

	//Total message length should exclude the first uint32_t that represents the totalMessageLength
	uint32_t totalMessageLength = 0;

	//Lock the sockets read mutex
	//std::lock_guard<std::mutex>(clientSocket->readMutex);

	//Get the total messageLength
	currentlyReadBytes = NetworkSocketReceive(clientSocket->handle, (void*)&totalMessageLength, sizeof(uint32_t), 0);

	//Check if the socket has been closed
	if(currentlyReadBytes == 0)
	{
		fprintf(stderr, "ReadData::The socket has been closed!\n");
		return;
	}
	//Check if an error has occured
	else if(currentlyReadBytes == -1)
	{
		fprintf(stderr,"ReadData::Error occured when trying to read from a socket!\n");
		return;
	}

	//If we get up to here we have successfully read from the socket
	//allocate a buffer that is big enough to store the whole message
	char* buffer = (char*)malloc(sizeof(char) * totalMessageLength);

	//Put the total message length into the buffer
	memcpy(buffer, &totalMessageLength, sizeof(uint32_t));

	//Keep reading until we have the whole message
	while(currentlyReadBytes < totalMessageLength)
	{
		int32_t tempNumberOfBytesRecv = NetworkSocketReceive(clientSocket->handle, (void*)(buffer + currentlyReadBytes),
				totalMessageLength - currentlyReadBytes, 0);

		//Was the socket closed?
		if(tempNumberOfBytesRecv == 0)
		{
			fprintf(stderr, "ReadData::The socket has been closed!\n");
			return;
		}
		//Check if an error has occured
		else if(tempNumberOfBytesRecv == -1)
		{
			fprintf(stderr,"ReadData::Error occured when trying to read from a socket!\n");
			return;
		}

		//Everything went ok if we got here
		//Update the number of bytes received
		currentlyReadBytes += tempNumberOfBytesRecv;
	}

	//Create a network command object to represent a message that come in over the network
	NetworkCommand* networkCommand = BufferToNetworkCommand(clientSocket, buffer, totalMessageLength);

	//Free the buffer, we don't need it anymore
	free(buffer);

	//Convert the network command into a local command
	Command* localCommand = NetworkCommandToLocalCommand(networkCommand);

	//This isn't needed anymore
	delete networkCommand;

	//Put the local command in the server action buffer
	serverActionQueue->addCommand(localCommand);

}











NetworkCommand* BufferToNetworkCommand(Socket* socket, char* buffer, uint32_t bufferLength)
{
	//FYI: Total length = bufferLength

	uint16_t networkCommandType =	*((uint16_t*)(buffer + sizeof(uint32_t)));

	//Allocate enough space for the actual command data and copy it over
	uint32_t offsetToData = sizeof(uint32_t) + sizeof(uint16_t);
	uint32_t networkDataLength =  bufferLength - offsetToData;
	char* networkCommandData = (char*)malloc(networkDataLength);
	memcpy(networkCommandData, buffer + offsetToData, networkDataLength);

	NetworkCommand* result = new NetworkCommand(socket, bufferLength, networkCommandType, networkCommandData, networkDataLength);

	return result;
}


Command* NetworkCommandToLocalCommand(NetworkCommand* networkCommand)
{
	Command* result;

	NetworkCommandType networkCommandType = (NetworkCommandType)networkCommand->commandType;

	switch(networkCommandType)
	{
		case NETWORK_LOGIN:
			return NetworkCommandToLoginCommand(networkCommand);
		case NETWORK_UNSET:
			return NULL;
		default:
			return NULL;
	}

	return result;
}



LoginCommand* NetworkCommandToLoginCommand(NetworkCommand* networkCommand)
{
	//6 bytes = uint32 + uint16
	const uint32_t usernameLengthOffset = 6;

	uint8_t usernameLength = *(uint8_t*)(networkCommand->commandData + usernameLengthOffset);

	//Assumes username is null terminated, which it should be!
	char usernameBuffer[256];
	memcpy(usernameBuffer, networkCommand->commandData + 7, usernameLength);


	std::string* username = new std::string(usernameBuffer);
	LoginCommand* loginCommand = new LoginCommand(networkCommand->socket, (NetworkCommandType)networkCommand->commandType, username);

	return loginCommand;
}



void AddWorkItemToNetworkThreadQueue(NetworkingThreadData* networkData, std::function<void()> workItem)
{
	{
		//Get the mutex
		std::lock_guard<std::mutex> workQueueLock(networkData->mutex);

		networkData->workQueue.push(workItem);
	}

	//Let the network queue know/wake up if sleeping
	networkData->conditionVar.notify_one();
}
*/

