/*
 * MasterMailbox.cpp
 *
 *  Created on: 23 Jan 2016
 *      Author: vincent
 */

#include "MasterMailbox.h"

MasterMailbox::MasterMailbox(ServerData* serverData, AcceptData* acceptData, NetworkData* networkData)
{
	acceptToServer = new AcceptToSeverMailbox(serverData, acceptData);
	serverToNetwork = new ServerToNetworkMailbox(serverData, networkData);
}

MasterMailbox::~MasterMailbox()
{
	delete acceptToServer;
	delete serverToNetwork;
}

