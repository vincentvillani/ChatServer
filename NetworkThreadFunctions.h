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


#include "Socket.h"
#include "ChatMessage.h"
#include "NetworkCommand.h"
#include "LoginCommand.h"
#include "ActionQueue.h"
#include "NetworkingThreadData.h"

//Functions that should be called by the network thread
//--------------------------------------------------------------------
void NetworkThreadMain(NetworkingThreadData* networkData);
void ProcessWorkQueue(NetworkingThreadData* networkData);
bool ConditionVariablePredicate(NetworkingThreadData* networkData);

void ReadData(ActionQueue* serverActionQueue, Socket* socketHandle);

NetworkCommand* BufferToNetworkCommand(Socket* socket, char* buffer, uint32_t bufferLength);
Command* NetworkCommandToLocalCommand(NetworkCommand* networkCommand);

LoginCommand* NetworkCommandToLoginCommand(NetworkCommand* networkCommand);

//--------------------------------------------------------------------


//Functions that should be called from the Server thread
//--------------------------------------------------------------------
void AddWorkItemToNetworkThreadQueue(NetworkingThreadData* networkData, std::function<void()> workItem);

//--------------------------------------------------------------------

#endif /* NETWORKTHREADFUNCTIONS_H_ */
