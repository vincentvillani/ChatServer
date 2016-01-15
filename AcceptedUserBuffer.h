/*
 * AcceptedSocketBuffer.h
 *
 *  Created on: 15 Jan 2016
 *      Author: vincent
 */

#ifndef ACCEPTEDUSERBUFFER_H_
#define ACCEPTEDUSERBUFFER_H_

#include <vector>
#include <mutex>

#include "User.h"

class AcceptedUserBuffer
{
	std::vector<User*> _acceptedUsers;
	std::mutex _mutex;

public:
	AcceptedUserBuffer();
	virtual ~AcceptedUserBuffer();

	void addUser(User* acceptedUser);
	void removeUser(int socketHandle);
};

#endif /* ACCEPTEDUSERBUFFER_H_ */
