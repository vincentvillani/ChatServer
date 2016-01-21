/*
 * NetworkCommand.cpp
 *
 *  Created on: 15 Jan 2016
 *      Author: vincentvillani
 */

#include "NetworkCommand.h"

NetworkCommand::NetworkCommand(int sockHandle, uint32_t networkTotalLength, uint16_t networkCommandType, char* networkCommandData, uint32_t networkCommandDataLength)
{
	socketHandle = sockHandle;
	totalLength = networkTotalLength;
	commandType = networkCommandType;
	commandData = networkCommandData;
	commandDataLength = networkCommandDataLength;
}

NetworkCommand::~NetworkCommand()
{
}

