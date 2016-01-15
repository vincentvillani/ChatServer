/*
 * AcceptedSocketBuffer.cpp
 *
 *  Created on: 15 Jan 2016
 *      Author: vincent
 */

#include "AcceptedUserBuffer.h"

#include <stdint.h>

AcceptedUserBuffer::AcceptedUserBuffer()
{
}

AcceptedUserBuffer::~AcceptedUserBuffer()
{
	for(uint32_t i = 0; i < _acceptedUsers.size(); ++i)
	{
		delete _acceptedUsers[i];
	}

	_acceptedUsers.clear();
}


void AcceptedUserBuffer::addUser(User* acceptedUser)
{
	std::lock_guard<std::mutex> lock(_mutex);

	_acceptedUsers.push_back(acceptedUser);
}


void AcceptedUserBuffer::removeUser(int socketHandle)
{
	std::lock_guard<std::mutex> lock(_mutex);

	for(uint32_t i = 0; i < _acceptedUsers.size(); ++i)
	{
		if(_acceptedUsers[i]->socket->handle == socketHandle)
		{
			_acceptedUsers.erase(_acceptedUsers.begin() + i);
			return;
		}
	}
}

