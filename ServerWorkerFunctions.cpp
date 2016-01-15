/*
 * ServerWorkerFunctions.cpp
 *
 *  Created on: 14 Jan 2016
 *      Author: vincentvillani
 */

#include "ServerWorkerFunctions.h"


#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Network.h"

void ReadData(Socket* clientSocket)
{

	printf("Reading data...\n");

	//The amount of data that has currently been read from the socket
	int32_t currentlyReadBytes = 0;

	//Total message length should exclude the first uint32_t that represents the totalMessageLength
	uint32_t totalMessageLength = 0;

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

