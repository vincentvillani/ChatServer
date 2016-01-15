/*
 * AcceptThreadFunctions.h
 *
 *  Created on: 16 Jan 2016
 *      Author: vincent
 */

#ifndef ACCEPTTHREADFUNCTIONS_H_
#define ACCEPTTHREADFUNCTIONS_H_


#include "Socket.h"
#include "AcceptedUserBuffer.h"

void acceptThreadMain(AcceptedUserBuffer* serverAcceptedUserBuffer, Socket* listeningSocket);


#endif /* ACCEPTTHREADFUNCTIONS_H_ */
