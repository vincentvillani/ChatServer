/*
 * Command.h
 *
 *  Created on: 15 Jan 2016
 *      Author: vincentvillani
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include "Socket.h"

enum CommandType
{
	UNSET_COMMAND = 0,
	LOGIN_COMMAND = 1,
	CHAT_COMMAND = 2
};

class Command
{

public:
	Command(Socket* clientSocket);
	virtual ~Command();

	CommandType commandType; //Type of command
	Socket* userSocket; //Who the command came from (which socket)

	virtual void operator()();
};

#endif /* COMMAND_H_ */
