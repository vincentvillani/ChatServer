/*
 * NetworkReadWriteBuffer.h
 *
 *  Created on: 21 Jan 2016
 *      Author: vincent
 */
#ifndef NETWORKREADWRITEBUFFER_H_
#define NETWORKREADWRITEBUFFER_H_

#include "NetworkDataBuffer.h"



class NetworkReadWriteBuffer
{
private:



public:
	NetworkReadWriteBuffer(int socketHandle);
	virtual ~NetworkReadWriteBuffer();

	NetworkDataBuffer* readBuffer;
	NetworkDataBuffer* writeBuffer;
	int socketHandle;

};

#endif /* NETWORKREADWRITEBUFFER_H_ */
