/*
 * ChatServer.cpp
 *
 *  Created on: 7 Jan 2016
 *      Author: vincent
 */

#include "ChatServer.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <sys/poll.h>

#define LISTENING_PORT_STRING "3490"


void acceptThreadMain(ChatServer* server, Socket* listeningSocket)
{
	while(true)
	{
		//printf("Accept thread: start loop\n");

		SOCKADDRSTORAGE* incomingSocketAddress = (SOCKADDRSTORAGE*)malloc(sizeof(SOCKADDRSTORAGE));
		socklen_t incomingSocketAddressSize = sizeof(incomingSocketAddress);

		//SOCKADDR* incomingSocketAddress = (SOCKADDR*)malloc(sizeof(SOCKADDR));


		int returnValue;

		returnValue = NetworkSocketAccept(listeningSocket->handle, (SOCKADDR*)incomingSocketAddress, &incomingSocketAddressSize);

		if(returnValue == -1)
		{
			//printf("Accept Socket error: %s\n", gai_strerror(returnValue));

			free(incomingSocketAddress);
			continue;
		}

		//Construct a socket object
		Socket* newClientSocket = new Socket(returnValue, (SOCKADDR*)incomingSocketAddress);

		{
			//Let the server know about it
			//Lock the mutex so you can add something to the vector containing the pending sockets
			std::lock_guard<std::mutex> pendingClientLock(server->acceptedSocketsBufferMutex);
			server->acceptedSocketsBuffer.push_back(newClientSocket);
			//server->acceptedSocketsBufferCV.notify_one();

			//printf("Accept thread: Socket Connected\n");
		}

		//relax for a bit
		std::this_thread::yield();
	}

}



ChatServer::ChatServer()
{
	listeningSocket = NULL;

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


	//Start the accept thread
	std::thread acceptThread(acceptThreadMain, this, listeningSocket);
	acceptThread.detach();

}

ChatServer::~ChatServer()
{
	if(listeningSocket != NULL)
		delete listeningSocket;

	//close and delete all the sockets
	for(auto i = clientSocketsMap.begin(); i != clientSocketsMap.end(); ++i)
	{
		delete i->second;
	}
}



void ChatServer::update()
{
	while(true)
	{
		//Handle any incoming connections within the server
		updateAccept();


		//pollClientSocketsForWrite();


		//Relax for a bit
		std::this_thread::yield();
	}
}


void ChatServer::updateAccept()
{
	//Does it look like we can get anything?
	if(pendingConnectionAvailable() == false)
		return;

	//Try to lock the mutex
	std::unique_lock<std::mutex> pendingConnectionLock(acceptedSocketsBufferMutex, std::try_to_lock);

	//Did we get the lock?
	if(pendingConnectionLock.owns_lock())
	{
		//wait_for returned true
		if(pendingConnectionAvailable())
		{
			printf("Server: pending connection available!\n");

			//We have the lock automatically
			transferPendingClientSockets();

			//Release the lock
			pendingConnectionLock.unlock();
		}
	}

}



void ChatServer::transferPendingClientSockets()
{
	//We should hold the accept lock prior to this point

	//Move the client sockets across

	for(uint32_t i = 0; i < acceptedSocketsBuffer.size(); ++i)
	{
		Socket* tempSocket = acceptedSocketsBuffer[i];

		clientSocketsMap[tempSocket->handle] = tempSocket;

		printf("Server thread: Accepted socket!\n");
	}

	//Remove all items within the buffer
	acceptedSocketsBuffer.clear();

	//printf("Server thread: Accepted socket!");
}


//This should be called under lock via wait_for
bool ChatServer::pendingConnectionAvailable()
{
	return acceptedSocketsBuffer.size();
}


void ChatServer::pollClientSocketsForRead()
{
	/*
	//Get the mutex lock

	uint32_t clientSocketLength = clientSocketsMap.size();

	if(clientSocketLength)
	{
		POLLFD* pollStructs = pollArray.getArrayWithSize(clientSocketLength);


		uint32_t index = 0;
		for(auto i = clientSocketsMap.begin(); i != clientSocketsMap.end(); ++i)
		{

			pollStructs[index].fd = i->first;
			pollStructs[index].events = POLLIN;
			//.revents should be already cleared by pollArray.getArrayWithSize()

			index += 1;
		}


		//Do the poll
		int numberOfSocketsReadyForRead = NetworkSocketPoll(pollStructs, clientSocketLength, 100);

		//An error has occured
		if(numberOfSocketsReadyForRead == -1)
		{
			fprintf(stderr, "ChatServer::PollClientSocketsForWrite: %s", strerror(errno));
			return;
		}

		//Go through
		if(numberOfSocketsReadyForRead > 0)
		{

		}

	}
	*/

}

