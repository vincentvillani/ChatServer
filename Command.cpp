/*
 * Command.cpp
 *
 *  Created on: 15 Jan 2016
 *      Author: vincentvillani
 */

#include "Command.h"

#include <stdio.h>

Command::Command(Socket* clientSocket, NetworkCommandType commandType)
{
	this->userSocket = clientSocket;
	this->commandType = commandType;
}

Command::~Command()
{
}


