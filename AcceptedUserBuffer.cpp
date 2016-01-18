/*
 * AcceptedSocketBuffer.cpp
 *
 *  Created on: 15 Jan 2016
 *      Author: vincent
 */

#include "AcceptedUserBuffer.h"

#include <stdint.h>

#include "PendingConnectedUserCommand.h"

AcceptedUserBuffer::AcceptedUserBuffer(ActionQueue* serverActionQueue)
{
	_serverActionQueue = serverActionQueue;
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

	//Add a pending connected user command, to let the server know that something is waiting in this buffer
	_serverActionQueue->addCommand(new PendingConnectedUserCommand(acceptedUser->socket, NETWORK_PENDING_CONNECTED_USER));
}



std::vector<User*> AcceptedUserBuffer::retrieveAndRemoveAllPendingUsers()
{
	std::lock_guard<std::mutex> lock(_mutex);

	std::vector<User*> result;
	result = std::move(_acceptedUsers);

	_acceptedUsers.clear();

	return result;
}

/*
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
*/
