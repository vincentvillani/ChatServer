/*
 * ServerToNetworkMailbox.cpp
 *
 *  Created on: 21 Jan 2016
 *      Author: vincent
 */

#include "ServerToNetworkMailbox.h"

#include "NetworkThreadFunctions.h"
#include "ServerThreadFunctions.h"


ServerToNetworkMailbox::ServerToNetworkMailbox(ServerData* serverData, NetworkData* networkData)
{
	_serverData = serverData;
	_networkData = networkData;
}


ServerToNetworkMailbox::~ServerToNetworkMailbox()
{
}

