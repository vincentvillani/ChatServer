/*
 * User.cpp
 *
 *  Created on: 15 Jan 2016
 *      Author: vincent
 */

#include "User.h"

User::User(Socket* socket, std::string* username)
{
	this->socket = socket;
	this->username = username;

	if(username == NULL)
		this->hasUsername = false;
	else
		this->hasUsername = true;
}

User::~User()
{
	if(socket != NULL)
		delete socket;

	if(username != NULL)
		delete username;
}

