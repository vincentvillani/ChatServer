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
#include "NetworkCommandType.h"

static void StartAllWork(NetworkData* networkData);
static void RemoveSocketFromMap(NetworkData* networkData, MasterMailbox* masterMailbox, int socketHandle);

static void PollSocketsAndDoIO(NetworkData* networkData, MasterMailbox* masterMailbox);

static void ReadSocket(NetworkData* networkData, MasterMailbox* masterMailbox , NetworkReadWriteBuffer* socketBuffer);

//Try and process a buffer
static void TryProcessReadBuffer(NetworkData* networkData, MasterMailbox* masterMailbox, NetworkDataBuffer* readBuffer, int socketHandle);
//When you have enough data in a read buffer, turn it into a network command
static void ReadBufferToNetworkCommand(NetworkData* networkData, MasterMailbox* masterMailbox, NetworkDataBuffer* readBuffer, int socketHandle);

static void ProcessLoginNetworkCommand(NetworkData* networkData, MasterMailbox* masterMailbox, NetworkDataBuffer* readBuffer, int socketHandle);

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
	masterMailbox->NetworkRemoveUserFromServerThread(socketHandle);
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
				fprintf(stderr, "PollSocketsAndDoIO: Unable to find a socket handle in the socketMap\n");
				continue;
			}

			NetworkReadWriteBuffer* readAndWriteBuffer = iterator->second;

			//Read the socket
			ReadSocket(networkData, masterMailbox, readAndWriteBuffer);
		}

		/*
		 *
		if(currentPollStruct->revents & POLLOUT)
		{
			//Write to the socket
		}
		*/
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




void TryProcessReadBuffer(NetworkData* networkData, MasterMailbox* masterMailbox, NetworkDataBuffer* readBuffer, int socketHandle)
{
	//There is nothing we can do, return
	if(readBuffer->currentMessageSize == 0 && readBuffer->bytesRead < 4)
		return;

	while(true)
	{
		//Check each time through the loop if there is enough data to process the next message
		if(readBuffer->currentMessageSize == 0 && readBuffer->bytesRead < 4)
			break;

		//Are we not currently waiting for a whole message and have enough data to know the length of the next message?
		if(readBuffer->currentMessageSize == 0 && readBuffer->bytesRead >= 4)
		{
			readBuffer->currentMessageSize = (uint32_t)*readBuffer->data;
		}


		//At this point we should know the length of the next message

		//Do we have enough to process it?
		if(readBuffer->currentMessageSize >= readBuffer->bytesRead)
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

	uint16_t messageType = (uint16_t)*(readBuffer->data + 4);

	//TODO: Actually implement this
	//TODO: UPDATE THE FIELDS ON THE READ BUFFER SO THAT TryProcessReadBuffer HAS UPDATED VALUES IF SOMETHING IS DONE
	//What is the message type
	switch(messageType)
	{
		case NETWORK_LOGIN:
			ProcessLoginNetworkCommand(networkData, masterMailbox, readBuffer, socketHandle);
			break;

		case NETWORK_CHAT_MESSAGE:
			break;

		case NETWORK_UNSET:
			break;

		default:
			fprintf(stderr, "ReadBufferToNetworkCommand: Received a network message type that is unset\n");
			break;

	}


}


void ProcessLoginNetworkCommand(NetworkData* networkData, MasterMailbox* masterMailbox, NetworkDataBuffer* readBuffer, int socketHandle)
{
	//uint8_t usernameLength = (uint8_t)(readBuffer->data + 6);

	//4 + 2 + 1 byte, ONLY WORKS IF C STRING IS NULL TERMINATED!
	std::string* username = new std::string(readBuffer->data + 7);

	//Send the data over to the server thread
	masterMailbox->NetworkUserLoginToServerThread(username, socketHandle);


	//Shift the readBuffer data across
	ShiftReadBufferData(readBuffer);
}



void ShiftReadBufferData(NetworkDataBuffer* readBuffer)
{
	//Move the data across
	memcpy(readBuffer->data, readBuffer->data + readBuffer->currentMessageSize, readBuffer->bytesRead - readBuffer->currentMessageSize);

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

		bool workToDo = networkData->conditionVariable.wait_for(workQueueLock, std::chrono::milliseconds(0), [&] {return networkData->workQueue.size();});

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

