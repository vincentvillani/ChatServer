/*
 * AcceptToSeverMailbox.h
 *
 *  Created on: 20 Jan 2016
 *      Author: vincent
 */

#ifndef ACCEPTTOSEVERMAILBOX_H_
#define ACCEPTTOSEVERMAILBOX_H_

#include <functional>
#include <condition_variable>

#include "User.h"
#include "ServerData.h"
#include "AcceptData.h"

class AcceptToSeverMailbox
{

private:


	ServerData* _serverData; //TODO: Who is responsible for deleting this?
	AcceptData* _acceptData;


public:

	AcceptToSeverMailbox(ServerData* serverData, AcceptData* acceptData);
	virtual ~AcceptToSeverMailbox();

	void AcceptThreadAddNewConnectedUser(User* user);
	void AcceptThreadConfirmShutdown();

	void ServerThreadAcceptThreadShutdown();
};

#endif /* ACCEPTTOSEVERMAILBOX_H_ */
