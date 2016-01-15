/*
 * LoginCommand.cpp
 *
 *  Created on: 15 Jan 2016
 *      Author: vincentvillani
 */

#include "LoginCommand.h"

LoginCommand::LoginCommand(Socket* clientSocket, std::string* loginUsername) : Command(clientSocket)
{
	this->username = loginUsername;
}

LoginCommand::~LoginCommand()
{
	if(username != NULL)
		delete username;
}

