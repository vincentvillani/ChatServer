/*
 * PendingConnectedUserCommand.cpp
 *
 *  Created on: 18 Jan 2016
 *      Author: vincent
 */

#include "PendingConnectedUserCommand.h"

PendingConnectedUserCommand::PendingConnectedUserCommand(Socket* clientSocket, NetworkCommandType commandType) : Command(clientSocket, commandType)
{
}

PendingConnectedUserCommand::~PendingConnectedUserCommand()
{

}

