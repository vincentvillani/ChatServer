/*
 * Debug.h
 *
 *  Created on: 24 Jan 2016
 *      Author: vincent
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include <mutex>

class Debug
{
public:
	Debug();
	virtual ~Debug();

	static std::mutex printMutex;
};

#endif /* DEBUG_H_ */
