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



int NetworkSocketSelect(int highestSocketHandlePlusOne, fd_set* readyToRead, fd_set* readyToWrite, fd_set* raisedException, struct timeval* timeout)
{
	return select(highestSocketHandlePlusOne, readyToRead, readyToWrite, raisedException, timeout);
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
