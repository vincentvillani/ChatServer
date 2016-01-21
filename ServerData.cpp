/*
 * ChatServer.cpp
 *
 *  Created on: 7 Jan 2016
 *      Author: vincent
 */


#include "ServerData.h"





ServerData::ServerData()
{
	threadShutdownNum = 0;
}



ServerData::~ServerData()
{
	//close and delete all users
	for(auto i = clientUsersMap.begin(); i != clientUsersMap.end(); ++i)
	{
		delete i->second;
	}
}

