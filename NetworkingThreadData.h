/*
 * NetworkingThreadData.h
 *
 *  Created on: 18 Jan 2016
 *      Author: vincent
 */

#ifndef NETWORKINGTHREADDATA_H_
#define NETWORKINGTHREADDATA_H_


#include <mutex>
#include <queue>
#include <functional>
#include <condition_variable>


class NetworkingThreadData
{

public:
	NetworkingThreadData();
	virtual ~NetworkingThreadData();

	std::mutex mutex; //This should be locked before anything is used
	std::condition_variable conditionVar; //Use this to wake from sleep when notified
	std::queue<std::function<void()>> workQueue; //Communication from other threads to the network thread should be done through here
	std::vector<int> sockets; 	//Sockets that this thread should pay attention too
};

#endif /* NETWORKINGTHREADDATA_H_ */
