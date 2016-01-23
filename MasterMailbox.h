/*
 * MasterMailbox.h
 *
 *  Created on: 23 Jan 2016
 *      Author: vincent
 */

#ifndef MASTERMAILBOX_H_
#define MASTERMAILBOX_H_

#include "ServerData.h"
#include "AcceptData.h"
#include "NetworkData.h"
#include "User.h"

#include "ServerThreadFunctions.h"
#include "NetworkThreadFunctions.h"
#include "AcceptThreadMain.h"

#include <string>

class MasterMailbox
{
	ServerData* _serverData;
	AcceptData* _acceptData;
	NetworkData* _networkData;

public:
	MasterMailbox(ServerData* serverData, AcceptData* acceptData, NetworkData* networkData);
	virtual ~MasterMailbox();


	void AcceptThreadAddNewConnectedUser(User* user);
	void AcceptThreadConfirmShutdown();

	void ServerThreadAcceptThreadShutdown();

	void ServerAddSocketToNetworkThread(int socketHandle); //Server->Network: Add a socket to the network map
	void NetworkRemoveUserFromServerThread(int socketHandle); //Network->Server: Remove a user from the server map because they have disconnected
	void NetworkUserLoginToServerThread(std::string* username, int socketHandle);
};

#endif /* MASTERMAILBOX_H_ */
