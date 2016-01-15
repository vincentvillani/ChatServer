/*
 * Command.h
 *
 *  Created on: 15 Jan 2016
 *      Author: vincentvillani
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include "Socket.h"
#include "NetworkCommandType.h"

class Command
{

public:
	Command(Socket* clientSocket, NetworkCommandType commandType);
	virtual ~Command();

	NetworkCommandType commandType; //Type of command
	Socket* userSocket; //Who the command came from (which socket)
};

#endif /* COMMAND_H_ */
