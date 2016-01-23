/*
 * Network.c
 *
 *  Created on: 7 Jan 2016
 *      Author: vincent
 */


#include "Network.h"


int NetworkGetAddressInfo(const char* hostname, const char* portOrServiceName, const ADDRINFO* hostNameInfo, ADDRINFO** results)
{
	return getaddrinfo(hostname, portOrServiceName, hostNameInfo, results);
}



void NetworkFreeAddressInfo(ADDRINFO* addressInfo)
{
	freeaddrinfo(addressInfo);
}


int NetworkSocketCreate(int IPVersion, int socketType)
{
	return socket(IPVersion, socketType, 0);
}


int NetworkSocketConnect(int socketHandle, SOCKADDR* socketAddress, socklen_t socketAddressLength)
{
	return connect(socketHandle, socketAddress, socketAddressLength);
}


int NetworkSocketBind(int socketHandle, SOCKADDR* socketAddress, socklen_t socketAddressLength)
{
	return bind(socketHandle, socketAddress, socketAddressLength);
}



int NetworkSocketClose(int socketHandle)
{
	return close(socketHandle);
}


int NetworkSocketListen(int socketHandle, int backlogNum)
{
	return listen(socketHandle, backlogNum);
}



int NetworkSocketAccept(int socketHandle, SOCKADDR* incomingSocketAddress, socklen_t* incomingSocketStructSize)
{
	return accept(socketHandle, incomingSocketAddress, incomingSocketStructSize);
}






int NetworkSocketReceive(int socketHandle, void* buffer, size_t bufferLength, int bitFlags)
{
	return recv(socketHandle, buffer, bufferLength, bitFlags);
}

int NetworkSocketSend(int socketHandle, void* buffer, size_t bufferLength, int bitFlags)
{
	return send(socketHandle, buffer, bufferLength, bitFlags);
}


/*
int NetworkSocketReceiveAll(Socket* socket, void** buffer, size_t bufferLength, int bitFlags)
{

	uint32_t numberOfBytesReceived = 0;
	uint32_t totalMessageLength = 0;

	//Check how long the TOTAL message is, the total length should be placed into totalMessageLength
	numberOfBytesReceived = NetworkSocketReceive(socket->handle, (void*)totalMessageLength, sizeof(uint32_t), 0);

	//TODO: HANDLE THE SOCKET CLOSING
	//Remember that the socket has closed!
	if(numberOfBytesReceived == 0)
	{
		printf("The socket has closed!\n");
		return numberOfBytesReceived;
	} //TODO: Handle an error on read
	else if(numberOfBytesReceived == -1)
	{
		printf("An error has occured on reading!\n");
		return numberOfBytesReceived;
	}

	//We have to resize our buffer to fit the required size
	if(totalMessageLength < bufferLength)
	{

	}

	while(numberOfBytesReceived < totalMessageLength)
	{
		numberOfBytesReceived += NetworkSocketReceive(socket->handle, )
	}


}
*/

int NetworkSocketSelect(int highestSocketHandlePlusOne, fd_set* readyToRead, fd_set* readyToWrite, fd_set* raisedException, struct timeval* timeout)
{
	return select(highestSocketHandlePlusOne, readyToRead, readyToWrite, raisedException, timeout);
}


int NetworkSocketPoll(struct pollfd* socketsToPoll, uint32_t numberOfSockets, int timeoutMs)
{
	return poll(socketsToPoll, numberOfSockets, timeoutMs);
}



int NetworkSocketSetNonBlock(int socketHandle)
{
	return fcntl(socketHandle, F_SETFL, O_NONBLOCK);
}


int NetworkSocketSetReusable(int socketHandle, bool isReusable)
{
	int resuableValue = isReusable;
	return setsockopt(socketHandle, SOL_SOCKET, SO_REUSEADDR, &resuableValue, sizeof(int));
}



