/*
 * ServerWorkerFunctions.h
 *
 *  Created on: 14 Jan 2016
 *      Author: vincentvillani
 */

#ifndef NETWORKTHREADFUNCTIONS_H_
#define NETWORKTHREADFUNCTIONS_H_

#include <stdint.h>
#include <functional>

#include "NetworkData.h"
#include "ServerToNetworkMailbox.h"

void NetworkThreadMain(NetworkData* networkData, ServerToNetworkMailbox* mailbox);

//Work functions
void NetworkThreadAddSocketToMap(NetworkData* network, int socketHandle);


#endif /* NETWORKTHREADFUNCTIONS_H_ */
