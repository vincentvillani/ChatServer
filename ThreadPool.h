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

void workerThreadLoop();


//Simple thread pool
class ThreadPool
{
	std::mutex _mutex;
	std::queue<int> _workQueue;
	std::vector<std::thread*> _threads;

public:
	ThreadPool();
	virtual ~ThreadPool();

};

#endif /* THREADPOOL_H_ */
