/*
 * PollArray.cpp
 *
 *  Created on: 13 Jan 2016
 *      Author: vincent
 */

#include "PollArray.h"

#include <string.h>

PollArray::PollArray()
{
	_pollArrayLength = 0;
	_pollArray = NULL;
}

PollArray::~PollArray()
{
	if(_pollArray != NULL)
		free(_pollArray);
}



uint32_t PollArray:: getCurrentLength()
{
	return _pollArrayLength;
}


//Size is guaranteed to be at least size, possibly bigger
POLLFD* PollArray::getArrayWithSize(uint32_t size)
{
	if(size > _pollArrayLength)
	{
		free(_pollArray);
		_pollArray = (POLLFD*)malloc(sizeof(POLLFD) * size);
	}

	memset(_pollArray, size, sizeof(POLLFD));
	return _pollArray;
}

