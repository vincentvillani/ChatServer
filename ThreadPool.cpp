/*
 * ThreadPool.cpp
 *
 *  Created on: 11 Jan 2016
 *      Author: vincentvillani
 */

#include "ThreadPool.h"



ThreadPool::ThreadPool()
{
	//Setup the worker threads
	uint32_t supportedThreads = std::thread::hardware_concurrency();

	if(supportedThreads == 0)
		supportedThreads = 2;

	for(uint32_t i = 0; i < supportedThreads; ++i)
	{
		std::thread workerThread(&ThreadPool::workerThreadLoop, this);
		workerThread.detach();
	}
}

ThreadPool::~ThreadPool()
{
}


void ThreadPool::addToWorkQueue(std::function<void()> functor)
{
	std::lock_guard<std::mutex> workLock(_workQueuemutex);

	_workQueue.push(functor);
}


void ThreadPool::workerThreadLoop()
{
	while(true)
	{
		std::function<void()> workItem;

		{
			std::unique_lock<std::mutex> workQueueLock(_workQueuemutex);

			//Is there any work to do?
			_workCV.wait(workQueueLock, [&]{return _workQueue.size();});

			//At this point there is some work to do
			//We already have the lock so grab something out of the queue
			workItem = _workQueue.front();
			_workQueue.pop();

			//The unique lock will be released
		}

		//Start the work item
		workItem();
	}

}


