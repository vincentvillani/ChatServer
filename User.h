/*
 * User.h
 *
 *  Created on: 15 Jan 2016
 *      Author: vincent
 */

#ifndef USER_H_
#define USER_H_

#include <string>

#include "Socket.h"


class User
{


public:
	User(Socket* socket, std::string* username);
	virtual ~User();

	Socket* socket;
	bool hasUsername; //Hacky, think of changing this?
	std::string* username;
};

#endif /* USER_H_ */
