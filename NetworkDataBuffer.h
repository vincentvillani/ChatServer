/*
 * NetworkDataBuffer.h
 *
 *  Created on: 21 Jan 2016
 *      Author: vincent
 */

#ifndef NETWORKDATABUFFER_H_
#define NETWORKDATABUFFER_H_

#include <stdint.h>

class NetworkDataBuffer
{


public:


	uint32_t currentMessageSize;
	uint32_t bytesRead;
	uint32_t bufferSize;
	char* data;


	NetworkDataBuffer(uint32_t bufferSize);
	virtual ~NetworkDataBuffer();

	int remainingStorageCapacity();
	void grow();
};

#endif /* NETWORKDATABUFFER_H_ */
