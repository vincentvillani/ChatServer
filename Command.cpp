/*
 * Command.cpp
 *
 *  Created on: 15 Jan 2016
 *      Author: vincentvillani
 */

#include "Command.h"

#include <stdio.h>

Command::Command(Socket* clientSocket)
{
	this->userSocket = clientSocket;
	this->commandType = UNSET_COMMAND;
}

Command::~Command()
{
}


void Command::operator()()
{
	fprintf(stderr, "Operator() not overloaded!!\n");
}

