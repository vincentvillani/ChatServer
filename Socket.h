/*
 * Socket.h
 *
 *  Created on: 7 Jan 2016
 *      Author: vincent
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include <sys/types.h>
#include <sys/socket.h>



//#include <sys/select.h>
//#include <netdb.h>
//#include <unistd.h>
//#include <stdbool.h>

//#include <sys/fcntl.h>



typedef struct sockaddr SOCKADDR;


class Socket
{
public:
	Socket(int handle, SOCKADDR* address);
	virtual ~Socket();

	int handle;
	SOCKADDR* address;
};

#endif /* SOCKET_H_ */
