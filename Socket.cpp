/*
 * Socket.cpp
 *
 *  Created on: 7 Jan 2016
 *      Author: vincent
 */

#include "Socket.h"

#include <stdlib.h>

#include "Network.h"

Socket::Socket(int handle, SOCKADDR* address)
{
	this->handle = handle;
	this->address = address;
}

Socket::~Socket()
{
	NetworkSocketClose(handle);
	free(address);
}

