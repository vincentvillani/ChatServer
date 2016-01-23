/*
 * ServerToNetworkMailbox.h
 *
 *  Created on: 21 Jan 2016
 *      Author: vincent
 */

#ifndef SERVERTONETWORKMAILBOX_H_
#define SERVERTONETWORKMAILBOX_H_


#include "ServerData.h"
#include "NetworkData.h"

class ServerToNetworkMailbox
{
private:
	ServerData* _serverData;
	NetworkData* _networkData;


public:
	ServerToNetworkMailbox(ServerData* serverData, NetworkData* networkData);
	virtual ~ServerToNetworkMailbox();

};

#endif /* SERVERTONETWORKMAILBOX_H_ */
