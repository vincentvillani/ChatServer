/*
 * AcceptThreadFunctions.cpp
 *
 *  Created on: 16 Jan 2016
 *      Author: vincent
 */


#include <thread>

#include "AcceptThreadFunctions.h"
#include "Network.h"



void acceptThreadMain(AcceptedUserBuffer* serverAcceptedUserBuffer, Socket* listeningSocket)
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
			//Place the new user in the servers accepted socket buffer
			serverAcceptedUserBuffer->addUser(new User(newClientSocket, NULL));
		}

		//relax for a bit
		std::this_thread::yield();
	}
}

