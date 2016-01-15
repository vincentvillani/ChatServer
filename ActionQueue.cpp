/*
 * ActionQueue.cpp
 *
 *  Created on: 15 Jan 2016
 *      Author: vincent
 */

#include "ActionQueue.h"

#include <stdint.h>

ActionQueue::ActionQueue()
{

}

ActionQueue::~ActionQueue()
{
	//Delete all the items in the action queue
	while(_actionQueue.size())
	{
		delete _actionQueue.front();
		_actionQueue.pop();
	}
}


bool ActionQueue::containsCommands()
{
	//Don't worry about locking the mutex, if this is true, we will try and lock the mutex before taking a work item
	return _actionQueue.size();
}


Command* ActionQueue::getNextCommand()
{
	//Lock the mutex
	std::lock_guard<std::mutex> actionQueueLock(_mutex);

	Command* result = _actionQueue.front();
	_actionQueue.pop();

	return result;
}


void ActionQueue::addCommand(Command* command)
{
	//Lock the mutex
	std::lock_guard<std::mutex> actionQueueLock(_mutex);

	_actionQueue.push(command);
}
