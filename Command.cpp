/*
 * Command.cpp
 *
 *  Created on: 15 Jan 2016
 *      Author: vincentvillani
 */

#include "Command.h"

Command::Command(Socket* clientSocket)
{
	this->userSocket = clientSocket;
	this->commandType = UNSET_COMMAND;
}

Command::~Command()
{
}

