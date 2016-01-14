/*
 * ServerWorkerFunctions.cpp
 *
 *  Created on: 14 Jan 2016
 *      Author: vincentvillani
 */

#include "ServerWorkerFunctions.h"


#include <stdio.h>
#include <stdlib.h>

#include "Network.h"

void ReadData(Socket* clientSocket)
{
	printf("Reading data...\n");

	//The amount of data that has currently been read from the socket
	uint32_t currentlyReadBytes = 0;

	//Total message length should exclude the first uint32_t that represents the totalMessageLength
	uint32_t totalMessageLength = 0;
	//uint32_t usernameLength = 0;
	//uint32_t chatMessageLength = 0;

	//Lock the sockets read mutex
	std::lock_guard<std::mutex>(clientSocket->readMutex);

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

	//Reset this value, totalMessageLength excludes the first uint32_t
	currentlyReadBytes = 0;

	//Keep reading until we have the whole message
	while(currentlyReadBytes < totalMessageLength)
	{
		uint32_t tempNumberOfBytesRecv = NetworkSocketReceive(clientSocket->handle, (void*)(buffer + currentlyReadBytes),
				totalMessageLength - currentlyReadBytes, 0);

		//Was the socket closed or was there an error?
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

	//We should have the whole message by now


	ChatMessage* receivedMessage = ChatMessageFromBuffer(buffer);

	printf("User: %s\n", receivedMessage->userName.c_str());
	printf("Message: %s\n", receivedMessage->chatMessage.c_str());
}


ChatMessage* ChatMessageFromBuffer(char* buffer)
{
	//The first 4 bytes of the buffer should be the user length
	uint32_t usernameLength;
	memcpy(&usernameLength, buffer, sizeof(uint32_t));

	//uint32_t for the username length and the username's length
	//uint32_t messageLength = (uint32_t)buffer + usernameLength + sizeof(uint32_t);


	uint32_t usernameStartIndex = sizeof(uint32_t);
	uint32_t messageStartIndex = usernameStartIndex + usernameLength + sizeof(uint32_t);

	ChatMessage* message = new ChatMessage();

	message->userName = buffer + usernameStartIndex;
	message->chatMessage = buffer + messageStartIndex;

	return message;
}


