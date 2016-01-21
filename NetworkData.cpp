/*
 * NetworkData.cpp
 *
 *  Created on: 21 Jan 2016
 *      Author: vincent
 */

#include "NetworkData.h"

NetworkData::NetworkData()
{
	shouldContinue = true;
}

NetworkData::~NetworkData()
{
	for(auto i = socketHandleMap.begin(); i != socketHandleMap.end(); ++i)
	{
		NetworkReadWriteBuffer* currentBuffer = i->second;
		delete currentBuffer;
	}
}

