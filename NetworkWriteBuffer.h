/*
 * NetworkWriteBuffer.h
 *
 *  Created on: 23 Jan 2016
 *      Author: vincent
 */

#ifndef NETWORKWRITEBUFFER_H_
#define NETWORKWRITEBUFFER_H_

#include <stdint.h>

class NetworkWriteBuffer
{
public:

	int socketHandle;
	char* data;
	uint32_t totalBufferSize;
	uint32_t sentBytes;
	const uint16_t messageType;

	NetworkWriteBuffer(uint32_t bufferSize, char* data, int socketHandle, const uint16_t messageType);
	virtual ~NetworkWriteBuffer();

	uint32_t remainingBytesToSend();


};

#endif /* NETWORKWRITEBUFFER_H_ */
