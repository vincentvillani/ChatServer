/*
 * NetworkingThreadData.cpp
 *
 *  Created on: 18 Jan 2016
 *      Author: vincent
 */

#include "NetworkingThreadData.h"

#include <stdint.h>

#include "Network.h"

NetworkingThreadData::NetworkingThreadData()
{


}

NetworkingThreadData::~NetworkingThreadData()
{
	//Close all the connections and free the file descriptors at the OS level
	for(uint32_t i = 0; i < sockets.size(); ++i)
		NetworkSocketClose(sockets[i]);
}

