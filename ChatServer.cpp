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

		struct sockaddr_storage* incomingSocketAddress = (struct sockaddr_storage*)malloc(sizeof(struct sockaddr_storage));
		socklen_t incomingSocketAddressSize = sizeof(incomingSocketAddress);

		//SOCKADDR* incomingSocketAddress = (SOCKADDR*)malloc(sizeof(SOCKADDR));


		int returnValue;

		returnValue = NetworkSocketAccept(listeningSocket->handle, (SOCKADDR*)incomingSocketAddress, &incomingSocketAddressSize);

		if(returnValue == -1)
		{
			printf("Accept Socket error: %s\n", gai_strerror(returnValue));

			free(incomingSocketAddress);
			continue;
		}

		//Construct a socket object
		Socket* newClientSocket = new Socket(returnValue, (SOCKADDR*)incomingSocketAddress);

		//Let the server know about it
		//Lock the mutex so you can add something to the vector containing the pending sockets
		std::lock_guard<std::mutex> pendingClientLock(server->pendingClientSocketMutex);
		server->pendingClientSockets.push_back(newClientSocket);
		server->pendingClientSocketCV.notify_one();

		printf("Accept: Socket Connected\n");
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
	delete listeningSocket;

	for(uint32_t i = 0; i < clientSockets.size(); ++i)
	{
		delete clientSockets[i];
	}
}



void ChatServer::update()
{
	while(true)
	{
		std::unique_lock<std::mutex> pendingConnectionLock(pendingClientSocketMutex);

		bool pendingSocketAvailable = pendingClientSocketCV.wait_for(pendingConnectionLock, std::chrono::milliseconds(0),
				std::bind(&ChatServer::pendingConnectionAvailable, this));

		if(pendingSocketAvailable)
		{
			//We have the lock automatically
			transferPendingClientSockets();

			//Release the lock
			pendingConnectionLock.unlock();
		}

		pollClientSocketsForWrite();

	}
}



void ChatServer::transferPendingClientSockets()
{
	//Lock for the client servers
	std::lock_guard<std::mutex> clientSocketLock(clientSocketMutex);

	//We should hold both locks at this point
	//Move the client sockets across
	for(uint32_t i = 0; i < pendingClientSockets.size(); ++i)
	{
		Socket* tempSocket = pendingClientSockets[i];
		pendingClientSockets.erase(pendingClientSockets.begin() + i);

		clientSockets.push_back(tempSocket);
	}

	//Lock will be automatically removed
}


//This should be called under lock via wait_for
bool ChatServer::pendingConnectionAvailable()
{
	return pendingClientSockets.size();
}


void ChatServer::pollClientSocketsForWrite()
{
	char* helloWorldString = "Hello World";


	std::lock_guard<std::mutex> lockGuard(clientSocketMutex);

	if(clientSockets.size())
	{
		struct pollfd ufds[1];
		ufds[0].fd = clientSockets[0]->handle;
		ufds[0].events = POLLOUT;
		ufds[0].revents = 0;

		NetworkSocketPoll(ufds, 1, 0);

		if(ufds[0].revents == POLLOUT)
		{
			NetworkSocketSend(clientSockets[0]->handle, helloWorldString, 12, 0);

			NetworkSocketClose(clientSockets[0]->handle);
			clientSockets.erase(clientSockets.begin());
		}
	}

}

