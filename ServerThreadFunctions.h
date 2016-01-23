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

#include <string>

class MasterMailbox; //Forward dec

void ServerMain(ServerData* serverData, MasterMailbox* masterMailbox);

void ServerHandleNewUser(User* user, ServerData* server, MasterMailbox* masterMailbox);
void ServerRemoveUser(ServerData* server, int socketHandle);

void ServerHandleUsername(ServerData* server, MasterMailbox* masterMailbox, int socketHandle, std::string* username);

void ServerShutdownAllThreads(MasterMailbox* masterMailbox);
void ServerThreadShutdown(ServerData* server);

#endif /* SERVERTHREADFUNCTIONS_H_ */
