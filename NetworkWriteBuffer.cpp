/*
 * NetworkWriteBuffer.cpp
 *
 *  Created on: 23 Jan 2016
 *      Author: vincent
 */

#include "NetworkWriteBuffer.h"

#include <stdlib.h>

NetworkWriteBuffer::NetworkWriteBuffer(uint32_t bufferSize, char* data, int socketHandle, const uint16_t messageType) : messageType(messageType)
{
	this->totalBufferSize = bufferSize;
	this->sentBytes = 0;
	this->data = data;
	this->socketHandle = socketHandle;
	//this->messageType = messageType;
}

NetworkWriteBuffer::~NetworkWriteBuffer()
{
	free(data);
}

uint32_t NetworkWriteBuffer::remainingBytesToSend()
{
	return totalBufferSize - sentBytes;
}
