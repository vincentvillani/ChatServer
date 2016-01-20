/*
 * ChatServer.cpp
 *
 *  Created on: 7 Jan 2016
 *      Author: vincent
 */


#include "ServerData.h"





ServerData::ServerData()
{
}



ServerData::~ServerData()
{
	//close and delete all users
	for(auto i = clientUsersMap.begin(); i != clientUsersMap.end(); ++i)
	{
		delete i->second;
	}
}




/*

ServerData::ServerData()
{
	_listeningSocket = NULL;

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
	_listeningSocket = new Socket(listenerSocketHandle, NULL);

	//Start listening to be able to accept incoming connections, at max twenty client sockets can be in the accept buffer at a time
	returnValue = NetworkSocketListen(_listeningSocket->handle, 20);

	if(returnValue == -1)
	{
		printf("NetworkSocketListen Error: %s\n", gai_strerror(returnValue));
		exit(2);
	}



	//Start the accept thread
	std::thread acceptThread(acceptThreadMain, &_acceptedUserBuffer, _listeningSocket);
	acceptThread.detach();

	//Start networking thread
	_networkingThreadData = new NetworkingThreadData();
	std::thread networkingThread(NetworkThreadMain, _networkingThreadData);
	networkingThread.detach();


}

ServerData::~ServerData()
{
	if(_listeningSocket != NULL)
		delete _listeningSocket;

	if(_networkingThreadData != NULL)
	{
		delete _networkingThreadData;
	}


}



void ChatServer::update()
{
	while(true)
	{
		std::unique_lock<std::mutex> actionQueueLock(_actionQueue.mutex);

		//Wait for something to do
		_actionQueue.conditionVariable.wait(actionQueueLock, [&]{return _actionQueue.containsCommands();});



		//Break at some point?
	}
}


void ChatServer::acceptNewUser()
{

	std::vector<User*> newUsers = _acceptedUserBuffer.retrieveAndRemoveAllPendingUsers();

	//
	for(uint32_t i = 0; i < newUsers.size(); ++i)
	{
		_clientUsersMap.insert(std::pair<int, User*> (newUsers[i]->socket->handle, newUsers[i]));

		//Let the server know about this new user

	}

}




void ChatServer::pollClientSocketsForRead()
{

	uint32_t clientSocketLength = _clientUsersMap.size();

	if(clientSocketLength == 0)
		return;


	//Get the total number of client sockets and see if there is any data waiting on them
	POLLFD* pollStructs = _pollArray.getArrayWithSize(clientSocketLength);

	uint32_t index = 0;
	for(auto i = _clientUsersMap.begin(); i != _clientUsersMap.end(); ++i)
	{

		pollStructs[index].fd = i->first;
		pollStructs[index].events = POLLIN;
		//revents should be already cleared by pollArray.getArrayWithSize()

		index += 1;
	}


	//Do the poll
	int numberOfSocketsReadyForRead = NetworkSocketPoll(pollStructs, clientSocketLength, 0);

	//An error has occured
	if(numberOfSocketsReadyForRead == -1)
	{
		fprintf(stderr, "ChatServer::PollClientSocketsForWrite: %s", strerror(errno));
		return;
	}

	//Go through each socket and a read functor to the worker queue
	for(uint32_t i = 0; i < numberOfSocketsReadyForRead; ++i)
	{
		POLLFD* currentPollStruct = &pollStructs[i];

		//This socket is ready to read
		if(currentPollStruct->revents == POLLIN)
		{
			//printf("Data is here!\n");

			User* user = _clientUsersMap[currentPollStruct->fd];

			//Mutex is locked internally
			_workerPool.addToWorkQueue( std::bind(ReadData, &_actionQueue, user->socket) );
		}
	}
}
*/






