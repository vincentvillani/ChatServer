/*
 * PollArray.h
 *
 *  Created on: 13 Jan 2016
 *      Author: vincent
 */

#ifndef POLLARRAY_H_
#define POLLARRAY_H_

#include <stdint.h>
#include <stdlib.h>

#include "NetworkTypedefs.h"

class PollArray
{

private:
	uint32_t _pollArrayLength;
	POLLFD* _pollArray;

public:

	PollArray();
	virtual ~PollArray();

	uint32_t getCurrentLength(); //Current length allocated
	POLLFD* getArrayWithSize(uint32_t size); //Size is guaranteed to be at least size, possibly bigger
};

#endif /* POLLARRAY_H_ */
