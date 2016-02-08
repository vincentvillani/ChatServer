/*
 * ChatCommand.cpp
 *
 *  Created on: 8 Feb 2016
 *      Author: vincent
 */

#include "ChatCommand.h"

ChatCommand::ChatCommand(Socket* clientSocket, NetworkCommandType commandType, std::string cMessage) : Command(clientSocket, commandType)
{
	this->chatMessage = cMessage;
}

ChatCommand::~ChatCommand()
{
}

