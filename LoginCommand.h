/*
 * LoginCommand.h
 *
 *  Created on: 15 Jan 2016
 *      Author: vincentvillani
 */

#ifndef LOGINCOMMAND_H_
#define LOGINCOMMAND_H_

#include "Command.h"

#include <string>



class LoginCommand: public Command
{

public:
	LoginCommand(Socket* clientSocket, std::string* loginUsername);
	virtual ~LoginCommand();


	Socket* userSocket;
	std::string* username;

};

#endif /* LOGINCOMMAND_H_ */
