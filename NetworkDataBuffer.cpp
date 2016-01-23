/*
 * NetworkDataBuffer.cpp
 *
 *  Created on: 21 Jan 2016
 *      Author: vincent
 */

#include "NetworkDataBuffer.h"

#include <stdlib.h>
#include <string.h>

NetworkDataBuffer::NetworkDataBuffer(uint32_t bufferSize)
{
	currentMessageSize = 0;
	bytesRead = 0;
	data = (char*)malloc(bufferSize);
	this->bufferSize = bufferSize;

}

NetworkDataBuffer::~NetworkDataBuffer()
{
	if(data != NULL)
		free(data);
}


int NetworkDataBuffer::remainingStorageCapacity()
{
	return bufferSize - bytesRead;
}



void NetworkDataBuffer::grow()
{
	size_t newBufferSize = bufferSize * 1.5f;
	char* buffer = (char*)malloc(newBufferSize);

	//Copy the old data across, only the data we actually care about
	memcpy(buffer, data, bytesRead);

	free(data);
	bufferSize = newBufferSize;
	data = buffer;
}

