/*
 * ActionQueue.h
 *
 *  Created on: 15 Jan 2016
 *      Author: vincent
 */

#ifndef ACTIONQUEUE_H_
#define ACTIONQUEUE_H_

#include "Command.h"

#include <queue>
#include <mutex>
#include <condition_variable>





class ActionQueue
{

private:

	std::queue<Command*> _actionQueue;


public:
	ActionQueue();
	virtual ~ActionQueue();

	std::mutex mutex;
	std::condition_variable conditionVariable;

	bool containsCommands();
	Command* getNextCommand();
	void addCommand(Command* command);

};

#endif /* ACTIONQUEUE_H_ */
