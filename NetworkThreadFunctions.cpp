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
#include "NetworkCommandType.h"

static void StartAllWork(NetworkData* networkData);
static void RemoveSocketFromMap(NetworkData* networkData, MasterMailbox* masterMailbox, int socketHandle);

static void PollSocketsAndDoIO(NetworkData* networkData, MasterMailbox* masterMailbox);

static void ReadSocket(NetworkData* networkData, MasterMailbox* masterMailbox , NetworkReadWriteBuffer* socketBuffer);
static void PerformPendingWrites(NetworkData* networkData, MasterMailbox* masterMailbox, NetworkReadWriteBuffer* socketBuffer);

//Try and process a buffer
static void TryProcessReadBuffer(NetworkData* networkData, MasterMailbox* masterMailbox, NetworkDataBuffer* readBuffer, int socketHandle);
//When you have enough data in a read buffer, turn it into a network command
static void ReadBufferToNetworkCommand(NetworkData* networkData, MasterMailbox* masterMailbox, NetworkDataBuffer* readBuffer, int socketHandle);

static void ProcessUsernameChangedNetworkCommand(NetworkData* networkData, MasterMailbox* masterMailbox, NetworkDataBuffer* readBuffer, int socketHandle);
static void ProcessChatMessageNetworkCommand(NetworkData* networkData, MasterMailbox* masterMailbox, NetworkDataBuffer* readBuffer, int socketHandle);

static void ShiftReadBufferData(NetworkDataBuffer* readBuffer);


//Private functions
//-------------------------------------------
void StartAllWork(NetworkData* networkData)
{
	std::unique_lock<std::mutex> workQueueLock(networkData->mutex);

	while(networkData->workQueue.size())
	{
		std::function<void()> workItem = networkData->workQueue.front();
		networkData->workQueue.pop();

		//Unlock the mutex for others
		workQueueLock.unlock();

		//Do the work
		workItem();

		//Lock the mutex again before testing the condition again
		workQueueLock.lock();
	}


}


void RemoveSocketFromMap(NetworkData* networkData, MasterMailbox* masterMailbox, int socketHandle)
{
	//Remove it from the map on this thread, and call the mailbox to have it be removed on the server thread
	auto iterator = networkData->socketHandleMap.find(socketHandle);

	//Can't find it, it must have already been removed
	if(iterator == networkData->socketHandleMap.end())
		return;

	//Free the read/write buffer and remove from the map
	delete iterator->second;
	networkData->socketHandleMap.erase(iterator);

	//Let the server thread know that this user has disconnected
	masterMailbox->NetworkThreadRemoveUserFromServerThread(socketHandle);
}

void PollSocketsAndDoIO(NetworkData* networkData, MasterMailbox* masterMailbox)
{
	uint32_t clientSocketLength = networkData->socketHandleMap.size();

	if(clientSocketLength == 0)
		return;


	//Get the total number of client sockets and see if there is any data waiting on them
	POLLFD* pollStructs = networkData->pollArray.getArrayWithSize(clientSocketLength);

	uint32_t index = 0;
	for(auto i = networkData->socketHandleMap.begin(); i != networkData->socketHandleMap.end(); ++i)
	{

		pollStructs[index].fd = i->first;
		pollStructs[index].events = POLLIN | POLLOUT;
		//pollStructs[index].revents should be already cleared by pollArray.getArrayWithSize()

		index += 1;
	}


	//Do the poll
	int numberOfSocketsReadyForRead = NetworkSocketPoll(pollStructs, clientSocketLength, 50);

	//An error has occured
	if(numberOfSocketsReadyForRead == -1)
	{
		fprintf(stderr, "ChatServer::PollClientSocketsForWrite: %s", strerror(errno));
		return;
	}

	//Go through each socket and do what is required
	for(int i = 0; i < numberOfSocketsReadyForRead; ++i)
	{
		POLLFD* currentPollStruct = &pollStructs[i];

		//This socket is ready to read
		if(currentPollStruct->revents & POLLIN)
		{
			auto iterator = networkData->socketHandleMap.find(currentPollStruct->fd);

			if(iterator ==  networkData->socketHandleMap.end())
			{
				//fprintf(stderr, "PollSocketsAndDoIO: Unable to find a socket handle in the socketMap\n");
				continue;
			}

			NetworkReadWriteBuffer* readAndWriteBuffer = iterator->second;

			//Read the socket
			ReadSocket(networkData, masterMailbox, readAndWriteBuffer);
		}

		if(currentPollStruct->revents & POLLOUT)
		{
			//Find the associated buffer for the socket handle
			auto iterator = networkData->socketHandleMap.find(currentPollStruct->fd);

			if(iterator ==  networkData->socketHandleMap.end())
			{
				//fprintf(stderr, "PollSocketsAndDoIO: Unable to find a socket handle in the socketMap\n");
				continue;
			}

			NetworkReadWriteBuffer* readAndWriteBuffer = iterator->second;
			PerformPendingWrites(networkData, masterMailbox, readAndWriteBuffer);
		}
	}
}




void ReadSocket(NetworkData* networkData, MasterMailbox* masterMailbox, NetworkReadWriteBuffer* socketBuffer)
{
	int returnValue;

	bool connectionWillClose = false;

	//Read as much data as we can
	while(true)
	{
		//Make sure there is enough space to store new data
		if(socketBuffer->readBuffer->remainingStorageCapacity() == 0)
			socketBuffer->readBuffer->grow();

		//Read the socket
		returnValue = NetworkSocketReceive(socketBuffer->socketHandle, socketBuffer->readBuffer->data + socketBuffer->readBuffer->bytesRead,
				socketBuffer->readBuffer->remainingStorageCapacity(), 0);

		//Can't read anymore
		if(returnValue == -1)
		{
			break;
		}
		//Connection was closed by the client
		else if(returnValue == 0)
		{
			connectionWillClose = true;
			break;
		}
		//We must have read some data
		else
		{
			socketBuffer->readBuffer->bytesRead += returnValue;
		}
	}

	//See if there is a whole message ready, process and send it off to the server
	TryProcessReadBuffer(networkData, masterMailbox, socketBuffer->readBuffer, socketBuffer->socketHandle);

	//If connection will close, close it
	if(connectionWillClose)
		RemoveSocketFromMap(networkData, masterMailbox, socketBuffer->socketHandle);

}


void PerformPendingWrites(NetworkData* networkData, MasterMailbox* masterMailbox, NetworkReadWriteBuffer* socketBuffer)
{
	//Try to write as much as possible to the write buffer
	while(socketBuffer->writeBufferQueue.size())
	{
		NetworkWriteBuffer* currentWriteBuffer = socketBuffer->writeBufferQueue.front();
		char* currentBuffer = currentWriteBuffer->data;

		int bytesJustWritten = NetworkSocketSend(currentWriteBuffer->socketHandle, currentBuffer + currentWriteBuffer->sentBytes,
				currentWriteBuffer->remainingBytesToSend(), 0);

		//We can't write any more, lets just leave
		if(bytesJustWritten == -1)
		{
			break;
		}

		//Update the amount of bytes sent
		currentWriteBuffer->sentBytes += bytesJustWritten;

		//Have we sent this entire message?
		if(currentWriteBuffer->remainingBytesToSend() == 0)
		{
			//We have sent this whole message
			delete currentWriteBuffer;

			//Remove it from the queue
			socketBuffer->writeBufferQueue.pop();
		}
	}
}




void TryProcessReadBuffer(NetworkData* networkData, MasterMailbox* masterMailbox, NetworkDataBuffer* readBuffer, int socketHandle)
{
	//There is nothing we can do, return
	//if(readBuffer->currentMessageSize == 0 && readBuffer->bytesRead < 4)
	//	return;

	while(true)
	{
		//Check each time through the loop if there is enough data to process the next message
		if(readBuffer->currentMessageSize == 0 && readBuffer->bytesRead < 4)
			break;

		//Are we not currently waiting for a whole message and have enough data to know the length of the next message?
		if(readBuffer->currentMessageSize == 0 && readBuffer->bytesRead >= 4)
		{
			readBuffer->currentMessageSize = *((uint32_t*)readBuffer->data);
		}


		//At this point we should know the length of the next message

		//Do we have enough to process it?
		if(readBuffer->currentMessageSize <= readBuffer->bytesRead)
		{
			//Process it
			ReadBufferToNetworkCommand(networkData, masterMailbox, readBuffer, socketHandle);
		}
		else //Can't do anymore, break
			break;
	}

}



void ReadBufferToNetworkCommand(NetworkData* networkData, MasterMailbox* masterMailbox, NetworkDataBuffer* readBuffer, int socketHandle)
{

	uint16_t messageType = *(uint16_t*)(readBuffer->data + 4);

	//TODO: Actually implement this
	//TODO: UPDATE THE FIELDS ON THE READ BUFFER SO THAT TryProcessReadBuffer HAS UPDATED VALUES IF SOMETHING IS DONE
	//What is the message type
	switch(messageType)
	{
		case NETWORK_USERNAME:
			ProcessUsernameChangedNetworkCommand(networkData, masterMailbox, readBuffer, socketHandle);
			break;

		case NETWORK_CHAT_MESSAGE:
			ProcessChatMessageNetworkCommand(networkData, masterMailbox, readBuffer, socketHandle);
			break;

		case NETWORK_UNSET:
			break;

		default:
			fprintf(stderr, "ReadBufferToNetworkCommand: Received a network message type that is unset\n");
			break;

	}


}


void ProcessUsernameChangedNetworkCommand(NetworkData* networkData, MasterMailbox* masterMailbox, NetworkDataBuffer* readBuffer, int socketHandle)
{
	//uint8_t usernameLength = (uint8_t)(readBuffer->data + 6);

	//4 + 2 + 1 byte, ONLY WORKS IF C STRING IS NULL TERMINATED!
	std::string* username = new std::string(readBuffer->data + 7);

	//printf("Username: %s", username->c_str());

	//Send the data over to the server thread
	masterMailbox->NetworkThreadUserLoginToServerThread(username, socketHandle);


	//Shift the readBuffer data across
	ShiftReadBufferData(readBuffer);
}


void ProcessChatMessageNetworkCommand(NetworkData* networkData, MasterMailbox* masterMailbox, NetworkDataBuffer* readBuffer, int socketHandle)
{
	uint16_t usernameByteOffsetLength = *(uint16_t*)(readBuffer->data + 6);

	//Assumes the username is null terminated
	std::string username(readBuffer->data + 8);

	//4 + 2 + 2 + usernameDataLength + 4
	uint32_t chatMessageByteOffset = 12 + usernameByteOffsetLength; //4 + 2 + 4

	//Assumes the C String is NULL terminated
	std::string chatMessage(readBuffer->data + chatMessageByteOffset);

	//Send the data over to the server thread
	masterMailbox->NetworkThreadChatMessageToServerThread(username, chatMessage, socketHandle);

	//Shift the readBuffer across
	ShiftReadBufferData(readBuffer);
}


void ShiftReadBufferData(NetworkDataBuffer* readBuffer)
{
	//printf("%d", *(uint16_t*)(readBuffer->data + 19));
	//Move the data across
	memmove(readBuffer->data, readBuffer->data + readBuffer->currentMessageSize, readBuffer->bytesRead - readBuffer->currentMessageSize);

	//Update the buffer fields
	readBuffer->bytesRead -= readBuffer->currentMessageSize;
	readBuffer->currentMessageSize = 0;
}


//-------------------------------------------



void NetworkThreadMain(NetworkData* networkData, MasterMailbox* masterMailbox)
{
	while(networkData->shouldContinue)
	{
		std::unique_lock<std::mutex> workQueueLock (networkData->mutex);

		bool workToDo = networkData->conditionVariable.wait_for(workQueueLock, std::chrono::milliseconds(50), [&] {return networkData->workQueue.size();});

		if(workToDo)
		{
			workQueueLock.unlock();

			StartAllWork(networkData);
		}

		//Poll for sockets ready for reading or writing, and perform I/O if needed
		PollSocketsAndDoIO(networkData, masterMailbox);


	}


}


void NetworkThreadAddSocketToMap(NetworkData* network, int socketHandle)
{
	int returnValue = NetworkSocketSetNonBlock(socketHandle);

	if(returnValue == -1)
	{
		fprintf(stderr, "NetworkThreadAddSocketToMap: Unable to set socket to be non-blocking\n");
	}

	std::pair<int, NetworkReadWriteBuffer*> newPair(socketHandle, new NetworkReadWriteBuffer(socketHandle));
	network->socketHandleMap.insert(newPair);
}


void NetworkThreadStartSendingChatMessage(NetworkData* networkData, std::string username, std::string chatMessage, int socketHandle)
{
	auto iterator = networkData->socketHandleMap.find(socketHandle);

	if(iterator == networkData->socketHandleMap.end())
	{
		fprintf(stderr, "NetworkThreadStartSendingChatMessage: Unable to find the readWriteBuffer\n");
		return;
	}

	//Get this users read and write buffer
	NetworkReadWriteBuffer* readWriteBuffer = iterator->second;


	uint16_t usernameLength = username.size() + 1; //Plus one for null terminator
	uint32_t chatMessageLength = chatMessage.size() + 1; //Plus one for null terminator
	uint16_t messageType = NETWORK_CHAT_MESSAGE;

	//Total message length, message type, username length, username data length, chat message length, chat message data length
	uint32_t totalMessageByteSize = sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint16_t) + usernameLength + sizeof(uint32_t) + chatMessageLength;

	char* data = (char*)malloc(totalMessageByteSize);
	char terminator = '\0';

	uint32_t runningOffset = 0;

	memcpy(data, &totalMessageByteSize, sizeof(uint32_t)); //Total message length
	runningOffset += sizeof(uint32_t);

	memcpy(data + runningOffset, &messageType, sizeof(uint16_t)); //message type
	runningOffset += sizeof(uint16_t);

	memcpy(data + runningOffset, &usernameLength, sizeof(uint16_t));
	runningOffset += sizeof(uint16_t);

	memcpy(data + runningOffset, username.c_str(), username.size());
	runningOffset += username.size();

	memcpy(data + runningOffset, &terminator, 1);
	runningOffset += 1;

	memcpy(data + runningOffset, &chatMessageLength, sizeof(uint32_t)); //chat message length
	runningOffset += sizeof(uint32_t);

	memcpy(data + runningOffset, chatMessage.c_str(), chatMessage.size());
	runningOffset += chatMessage.size();

	memcpy(data + runningOffset, &terminator, 1);
	runningOffset += 1;

	//Create a write buffer and add it to the iobuffer queue
	NetworkWriteBuffer* networkWriteBuffer = new NetworkWriteBuffer(totalMessageByteSize, data, socketHandle, messageType);

	readWriteBuffer->writeBufferQueue.push(networkWriteBuffer);
}


