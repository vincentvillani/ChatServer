/*
 * AcceptToSeverMailbox.cpp
 *
 *  Created on: 20 Jan 2016
 *      Author: vincent
 */

#include "AcceptToSeverMailbox.h"

#include "ServerThreadFunctions.h"
#include "AcceptThreadMain.h"

AcceptToSeverMailbox::AcceptToSeverMailbox(ServerData* serverData, AcceptData* acceptData)
{
	_serverData = serverData;
	_acceptData = acceptData;
}

AcceptToSeverMailbox::~AcceptToSeverMailbox()
{

}




