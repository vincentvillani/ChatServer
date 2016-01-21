/*
 * NetworkData.h
 *
 *  Created on: 21 Jan 2016
 *      Author: vincent
 */

#ifndef NETWORKDATA_H_
#define NETWORKDATA_H_

#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>

#include "PollArray.h"
#include "NetworkReadWriteBuffer.h"

class NetworkData
{

public:

	bool shouldContinue;

	std::unordered_map<int, NetworkReadWriteBuffer*> socketHandleMap;

	std::queue<std::function<void()>> workQueue;
	std::mutex mutex;
	std::condition_variable conditionVariable;

	PollArray pollArray;

	NetworkData();
	virtual ~NetworkData();



};

#endif /* NETWORKDATA_H_ */
