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
#include "MasterMailbox.h"

void ServerMain(ServerData* serverData, MasterMailbox* masterMailbox);

void ServerHandleNewUser(ServerData* server, User* user);
void ServerRemoveUser(ServerData* server, int socketHandle);

void ServerShutdownAllThreads(MasterMailbox* masterMailbox);
void ServerThreadShutdown(ServerData* server);

#endif /* SERVERTHREADFUNCTIONS_H_ */
