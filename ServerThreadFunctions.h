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
void ServerRemoveUser(ServerData* server, MasterMailbox* masterMailbox, int socketHandle);

void ServerHandleUsername(ServerData* server, MasterMailbox* masterMailbox, std::string* username, int socketHandle);

void ServerHandleChatMessage(ServerData* server, MasterMailbox* masterMailbox, std::string username, std::string chatMessage, int socketHandle);

void ServerShutdownAllThreads(MasterMailbox* masterMailbox);
void ServerThreadShutdown(ServerData* server);

#endif /* SERVERTHREADFUNCTIONS_H_ */
