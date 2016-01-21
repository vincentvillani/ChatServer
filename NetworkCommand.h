/*
 * NetworkCommand.h
 *
 *  Created on: 15 Jan 2016
 *      Author: vincentvillani
 */

#ifndef NETWORKCOMMAND_H_
#define NETWORKCOMMAND_H_

#include <stdint.h>

#include "NetworkCommandType.h"


class NetworkCommand
{

public:
	NetworkCommand(int sockHandle, uint32_t networkTotalLength, uint16_t networkCommandType, char* networkCommandData, uint32_t networkCommandDataLength);
	virtual ~NetworkCommand();

	int socketHandle;
	uint32_t totalLength; //Including itself
	uint16_t commandType; //The type of command, type is actually a NetworkCommandType (cast to a uint16_t)
	char* commandData; //The commands actual data
	uint32_t commandDataLength; //The byte length of commandData

};

#endif /* NETWORKCOMMAND_H_ */
