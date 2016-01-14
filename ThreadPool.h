/*
 * ThreadPool.h
 *
 *  Created on: 11 Jan 2016
 *      Author: vincentvillani
 */

#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <thread>
#include <functional>

//Simple thread pool
class ThreadPool
{

public:

	std::mutex _workQueuemutex;
	std::queue< std::function<void()> > _workQueue;
	std::condition_variable _workCV;

	ThreadPool();
	virtual ~ThreadPool();

	void addToWorkQueue(std::function<void()> functor);

	void workerThreadLoop();

};

#endif /* THREADPOOL_H_ */
