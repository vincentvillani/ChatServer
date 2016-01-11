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

void workerThreadLoop();


//Simple thread pool
class ThreadPool
{
	std::mutex _mutex;
	std::queue _workQueue;
	std::vector _threads;

public:
	ThreadPool();
	virtual ~ThreadPool();
};

#endif /* THREADPOOL_H_ */
