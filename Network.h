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

//#include <netinet/in.h>

//#include <arpa/inet.h>


typedef struct addrinfo ADDRINFO;
typedef struct sockaddr SOCKADDR;

//#define ADDRINFO struct addrinfo
//#define SOCKADDR struct sockaddr

int NetworkGetAddressInfo(const char* hostname, const char* portOrServiceName, const ADDRINFO* hostNameInfo, ADDRINFO** results);
void NetworkFreeAddressInfo(ADDRINFO* addressInfo);

int NetworkSocketCreate(int IPVersion, int socketType);
int NetworkSocketBind(int socketHandle, SOCKADDR* socketAddress, socklen_t socketAddressLength);
int NetworkSocketClose(int socketHandle);
int NetworkSocketListen(int socketHandle, int backlogNum); //Tells the operating system that I am interested in listening to incoming connections to this socket - NON BLOCKING
int NetworkSocketAccept(int socketHandle, SOCKADDR* incomingSocketAddress, socklen_t* incomingSocketStructSize); //Accepts an incoming connection
int NetworkSocketSelect(int highestSocketHandlePlusOne, fd_set* readyToRead, fd_set* readyToWrite, fd_set* raisedException, struct timeval* timeout); //Poll if any sockets are ready/raised an exception

//ConfigureSockets
int NetworkSocketSetNonBlock(int socketHandle);
int NetworkSocketSetReusable(int socketHandle, bool isReusable);

#endif /* NETWORK_H_ */
