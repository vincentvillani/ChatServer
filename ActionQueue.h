/*
 * ActionQueue.h
 *
 *  Created on: 15 Jan 2016
 *      Author: vincent
 */

#ifndef ACTIONQUEUE_H_
#define ACTIONQUEUE_H_

#include <queue>
#include <mutex>

#include "Command.h"

class ActionQueue
{
	std::queue<Command*> _actionQueue;
	std::mutex _mutex;


public:
	ActionQueue();
	virtual ~ActionQueue();

	bool containsCommands();
	Command* getNextCommand();
	void addCommand(Command* command);
};

#endif /* ACTIONQUEUE_H_ */
