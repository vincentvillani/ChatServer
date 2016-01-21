/*
 * AcceptData.h
 *
 *  Created on: 21 Jan 2016
 *      Author: vincent
 */

#ifndef ACCEPTDATA_H_
#define ACCEPTDATA_H_

#include <condition_variable>
#include <functional>
#include <queue>
#include <mutex>

class AcceptData
{


public:
	AcceptData();
	virtual ~AcceptData();

	bool shouldContinue;

	std::condition_variable conditionVariable;
	std::queue<std::function<void()>> workQueue;
	std::mutex mutex;
};

#endif /* ACCEPTDATA_H_ */
