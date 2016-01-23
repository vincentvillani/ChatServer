/*
 * MasterMailbox.h
 *
 *  Created on: 23 Jan 2016
 *      Author: vincent
 */

#ifndef MASTERMAILBOX_H_
#define MASTERMAILBOX_H_

#include "AcceptToSeverMailbox.h"
#include "ServerToNetworkMailbox.h"

class MasterMailbox
{
public:
	MasterMailbox(ServerData* serverData, AcceptData* acceptData, NetworkData* networkData);
	virtual ~MasterMailbox();

	AcceptToSeverMailbox* acceptToServer;
	ServerToNetworkMailbox* serverToNetwork;
};

#endif /* MASTERMAILBOX_H_ */
