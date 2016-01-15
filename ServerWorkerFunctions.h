/*
 * ServerWorkerFunctions.h
 *
 *  Created on: 14 Jan 2016
 *      Author: vincentvillani
 */

#ifndef SERVERWORKERFUNCTIONS_H_
#define SERVERWORKERFUNCTIONS_H_

#include <stdint.h>

#include "Socket.h"
#include "ChatMessage.h"
#include "NetworkCommand.h"
#include "LoginCommand.h"
#include "ActionQueue.h"




void ReadData(ActionQueue* serverActionQueue, Socket* clientSocket);

NetworkCommand* BufferToNetworkCommand(Socket* socket, char* buffer, uint32_t bufferLength);
Command* NetworkCommandToLocalCommand(NetworkCommand* networkCommand);

LoginCommand* NetworkCommandToLoginCommand(NetworkCommand* networkCommand);

#endif /* SERVERWORKERFUNCTIONS_H_ */
