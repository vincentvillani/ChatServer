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

class NetworkData
{

public:

	bool shouldContinue;

	std::unordered_map<int, int> socketHandleMap;

	std::queue<std::function<void()>> workQueue;
	std::mutex mutex;
	std::condition_variable conditionVariable;

	NetworkData();
	virtual ~NetworkData();


};

#endif /* NETWORKDATA_H_ */
