/*
 * AcceptThreadFunctions.cpp
 *
 *  Created on: 16 Jan 2016
 *      Author: vincent
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "AcceptThreadMain.h"
#include "Network.h"


#define LISTENING_PORT_STRING "3490"


static Socket* acceptThreadInit()
{
	Socket* listeningSocket = NULL;

	//Setup the server
	int listenerSocketHandle;
	ADDRINFO hints;
	ADDRINFO* addressResults;
	int returnValue;

	//Clear the hints struct for use
	//And set it up
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; //Use either IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; //I want a TCP socket plz
	hints.ai_flags = AI_PASSIVE; //Use the IP address for this machine

	//Give me all the addresses that match these conditions
	returnValue = NetworkGetAddressInfo(NULL, LISTENING_PORT_STRING, &hints, &addressResults);

	//Check for errors
	if(returnValue == -1)
	{
		fprintf(stderr, "NetworkGetAddressInfo: %s\n", gai_strerror(returnValue));
		exit(1);
	}

	ADDRINFO* currentAddress;

	//Loop through all the results and bind to the first result that we can
	for(currentAddress = addressResults; currentAddress != NULL; currentAddress = currentAddress->ai_next)
	{
		//Try getting a handle to this address's socket
		listenerSocketHandle = NetworkSocketCreate(currentAddress->ai_family, currentAddress->ai_socktype);



		//We can't get a socket for whatever reason
		if(listenerSocketHandle == -1)
		{
			continue;
		}

		//Set the socket to be useable, if the program crashes for some reason, this will remove this error message
		NetworkSocketSetReusable(listenerSocketHandle, true);

		//Set the socket to be non-blocking
		//NetworkSocketSetNonBlock(listenerSocketHandle);

		//Try binding to this socket
		returnValue = NetworkSocketBind(listenerSocketHandle, currentAddress->ai_addr, currentAddress->ai_addrlen);

		//We are unable to bind to the socket
		if(returnValue == -1)
		{
			//Free the socket
			NetworkSocketClose(listenerSocketHandle);
			listenerSocketHandle = -1; //Reset the handle
			continue;
		}

		//If we get here we have got a handle to a socket and binded it correctly
		break;
	}

	//Free the addressinfo
	NetworkFreeAddressInfo(addressResults);

	//Unable to connect to anything
	if(listenerSocketHandle == -1)
	{
		fprintf(stderr, "Unable to get a handle and/or bind to it\n");
		exit(2);
	}

	//Set the listening socket
	listeningSocket = new Socket(listenerSocketHandle, NULL);

	//Start listening to be able to accept incoming connections, at max twenty client sockets can be in the accept buffer at a time
	returnValue = NetworkSocketListen(listeningSocket->handle, 20);

	if(returnValue == -1)
	{
		printf("NetworkSocketListen Error: %s\n", gai_strerror(returnValue));
		exit(2);
	}


	//Set it to be in non-block mode
	returnValue = NetworkSocketSetNonBlock(listeningSocket->handle);

	if(returnValue == -1)
	{
		fprintf(stderr, "Failed to set listening socket into a non-block mode\n");
		exit(2);
	}

	//If we get to this point we should be able to start accepting connections
	return listeningSocket;
}


void acceptThreadMain(AcceptData* acceptData, MasterMailbox* masterMailbox)
{

	//Do the setup for this threads work
	Socket* listeningSocket = acceptThreadInit();

	//Temp storage
	SOCKADDRSTORAGE tempIncomingSocketAddress;
	socklen_t tempIncomingSocketAddressSize = sizeof(tempIncomingSocketAddress);

	while(acceptData->shouldContinue)
	{
		std::unique_lock<std::mutex> workQueueLock(acceptData->mutex);

		bool receivedMessage = acceptData->conditionVariable.wait_for(workQueueLock,
				std::chrono::milliseconds(100), [&] {return acceptData->workQueue.size();});

		if(receivedMessage)
		{
			std::function<void()> workItem = acceptData->workQueue.front();
			acceptData->workQueue.pop();

			workQueueLock.unlock();

			workItem();
		}


		int returnValue;

		returnValue = NetworkSocketAccept(listeningSocket->handle, (SOCKADDR*)&tempIncomingSocketAddress, &tempIncomingSocketAddressSize);

		//Nothing to do
		if(returnValue == -1)
		{
			continue;
		}
		else //There is a new thread to accept
		{
			//Make a copy of the temp variable
			SOCKADDRSTORAGE* incomingSocketAddress = (SOCKADDRSTORAGE*)malloc(sizeof(SOCKADDRSTORAGE));
			memcpy(incomingSocketAddress, &tempIncomingSocketAddress, sizeof(SOCKADDRSTORAGE));

			//Construct a socket object
			Socket* newClientSocket = new Socket(returnValue, (SOCKADDR*)incomingSocketAddress);

			//Place the new user in the servers accepted socket buffer
			masterMailbox->AcceptThreadAddNewConnectedUser(new User(newClientSocket, NULL));
		}
	}

	delete listeningSocket;


	masterMailbox->AcceptThreadConfirmShutdown();

	//printf("Accept Thread is exiting\n!");


}

void acceptThreadShutdown(AcceptData* acceptData)
{
	acceptData->shouldContinue = false;
}

