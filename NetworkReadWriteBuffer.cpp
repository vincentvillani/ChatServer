/*
 * NetworkReadWriteBuffer.cpp
 *
 *  Created on: 21 Jan 2016
 *      Author: vincent
 */

#include "NetworkReadWriteBuffer.h"

#include <stdlib.h>

#define BUFFER_SIZE 1024 * 10

NetworkReadWriteBuffer::NetworkReadWriteBuffer(int socketHandle)
{
	this->socketHandle = socketHandle;
	readBuffer = new NetworkDataBuffer(BUFFER_SIZE);
	writeBuffer = new NetworkDataBuffer(BUFFER_SIZE);
}

NetworkReadWriteBuffer::~NetworkReadWriteBuffer()
{
	if(readBuffer != NULL)
		delete readBuffer;

	if(writeBuffer != NULL)
		delete writeBuffer;
}




