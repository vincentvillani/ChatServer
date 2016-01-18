/*
 * PendingConnectedUserCommand.h
 *
 *  Created on: 18 Jan 2016
 *      Author: vincent
 */

#ifndef PENDINGCONNECTEDUSERCOMMAND_H_
#define PENDINGCONNECTEDUSERCOMMAND_H_

#include "Command.h"

class PendingConnectedUserCommand: public Command
{
public:
	PendingConnectedUserCommand(Socket* clientSocket, NetworkCommandType commandType);
	virtual ~PendingConnectedUserCommand();
};

#endif /* PENDINGCONNECTEDUSERCOMMAND_H_ */
