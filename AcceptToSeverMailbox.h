/*
 * AcceptToSeverMailbox.h
 *
 *  Created on: 20 Jan 2016
 *      Author: vincent
 */

#ifndef ACCEPTTOSEVERMAILBOX_H_
#define ACCEPTTOSEVERMAILBOX_H_

#include <functional>

#include "User.h"
#include "ServerData.h"

class AcceptToSeverMailbox
{

private:


	ServerData* _serverData; //TODO: Who is responsible for deleting this?


public:

	AcceptToSeverMailbox(ServerData* serverData);
	virtual ~AcceptToSeverMailbox();

	void AcceptThreadAddNewConnectedUser(User* user);
};

#endif /* ACCEPTTOSEVERMAILBOX_H_ */
