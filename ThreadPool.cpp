/*
 * ThreadPool.cpp
 *
 *  Created on: 11 Jan 2016
 *      Author: vincentvillani
 */

#include "ThreadPool.h"

void workerThreadLoop()
{

}


ThreadPool::ThreadPool()
{
	//Setup the worker threads
	uint32_t supportedThreads = std::thread::hardware_concurrency();

	if(supportedThreads == 0)
		supportedThreads = 2;

	for(uint32_t i = 0; i < supportedThreads; ++i)
		_threads.push_back(new std::thread(workerThreadLoop));
}

ThreadPool::~ThreadPool()
{
	for(uint32_t i = 0; i < _threads.size(); ++i)
		delete _threads[i];
}

