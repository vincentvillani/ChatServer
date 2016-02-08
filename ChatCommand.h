/*
 * ChatCommand.h
 *
 *  Created on: 8 Feb 2016
 *      Author: vincent
 */

#ifndef CHATCOMMAND_H_
#define CHATCOMMAND_H_

#include "Command.h"

#include <string>

class ChatCommand: public Command
{

public:
	ChatCommand(Socket* clientSocket, NetworkCommandType commandType, std::string cMessage);
	virtual ~ChatCommand();

	std::string chatMessage;

};

#endif /* CHATCOMMAND_H_ */
