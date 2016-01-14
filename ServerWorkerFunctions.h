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

void ReadData(Socket* clientSocket);

ChatMessage* ChatMessageFromBuffer(char* buffer);
#endif /* SERVERWORKERFUNCTIONS_H_ */
