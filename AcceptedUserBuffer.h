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
#include "ActionQueue.h"

class AcceptedUserBuffer
{

private:
	std::vector<User*> _acceptedUsers;
	ActionQueue* _serverActionQueue; //Reference to the servers action queue, so we can place commands in there
	std::mutex _mutex;


public:
	AcceptedUserBuffer(ActionQueue* serverActionQueue);
	virtual ~AcceptedUserBuffer();

	void addUser(User* acceptedUser);
	std::vector<User*> retrieveAndRemoveAllPendingUsers();

};

#endif /* ACCEPTEDUSERBUFFER_H_ */
