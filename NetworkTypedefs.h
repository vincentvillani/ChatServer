/*
 * NetworkTypedefs.h
 *
 *  Created on: 13 Jan 2016
 *      Author: vincent
 */

#ifndef NETWORKTYPEDEFS_H_
#define NETWORKTYPEDEFS_H_

#include <sys/fcntl.h>
#include <sys/poll.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>


typedef struct addrinfo ADDRINFO;
typedef struct sockaddr SOCKADDR;
typedef struct sockaddr_storage SOCKADDRSTORAGE;
typedef struct pollfd POLLFD;




#endif /* NETWORKTYPEDEFS_H_ */
