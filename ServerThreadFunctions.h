/*
 * ServerThreadFunctions.h
 *
 *  Created on: 18 Jan 2016
 *      Author: vincent
 */

#ifndef SERVERTHREADFUNCTIONS_H_
#define SERVERTHREADFUNCTIONS_H_

#include "ServerData.h"
#include "User.h"

void ServerMain(ServerData* serverData);

void ServerHandleNewUser(ServerData* server, User* user);

#endif /* SERVERTHREADFUNCTIONS_H_ */
