/*
 * AcceptThreadFunctions.h
 *
 *  Created on: 16 Jan 2016
 *      Author: vincent
 */

#ifndef ACCEPTTHREADMAIN_H_
#define ACCEPTTHREADMAIN_H_


#include "Socket.h"
#include "AcceptToSeverMailbox.h"

//Waits in a loop for any incoming connection requests
void acceptThreadMain(AcceptToSeverMailbox* mailbox);


#endif /* ACCEPTTHREADMAIN_H_ */
