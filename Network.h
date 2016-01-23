/*
 * Network.h
 *
 *  Created on: 7 Jan 2016
 *      Author: vincent
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <unistd.h>
#include <stdbool.h>

#include <sys/fcntl.h>
#include <sys/poll.h>

#include "NetworkTypedefs.h"
#include "Socket.h"


int NetworkGetAddressInfo(const char* hostname, const char* portOrServiceName, const ADDRINFO* hostNameInfo, ADDRINFO** results);
void NetworkFreeAddressInfo(ADDRINFO* addressInfo);

int NetworkSocketCreate(int IPVersion, int socketType);
int NetworkSocketConnect(int socketHandle, SOCKADDR* socketAddress, socklen_t socketAddressLength);
int NetworkSocketBind(int socketHandle, SOCKADDR* socketAddress, socklen_t socketAddressLength);
int NetworkSocketClose(int socketHandle);
int NetworkSocketListen(int socketHandle, int backlogNum); //Tells the operating system that I am interested in listening to incoming connections to this socket - NON BLOCKING
int NetworkSocketAccept(int socketHandle, SOCKADDR* incomingSocketAddress, socklen_t* incomingSocketStructSize); //Accepts an incoming connection
int NetworkSocketReceive(int socketHandle, void* buffer, size_t bufferLength, int bitFlags);
int NetworkSocketSend(int socketHandle, void* buffer, size_t bufferLength, int bitFlags);

//For sending and receiving this should be used 99% of the time
//int NetworkSocketReceiveAll(Socket* socket, void* buffer, size_t bufferLength, int bitFlags);

//Polling sockets
int NetworkSocketSelect(int highestSocketHandlePlusOne, fd_set* readyToRead, fd_set* readyToWrite, fd_set* raisedException, struct timeval* timeout); //Poll if any sockets are ready/raised an exception
int NetworkSocketPoll(POLLFD* socketsToPoll, uint32_t numberOfSockets, int timeoutMs);

//ConfigureSockets
int NetworkSocketSetNonBlock(int socketHandle);
int NetworkSocketSetReusable(int socketHandle, bool isReusable);

#endif /* NETWORK_H_ */
