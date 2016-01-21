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

void NetworkThreadMain(NetworkData* networkData);

void NetworkThreadAddSocketToMap(NetworkData* network, int socketHandle);


#endif /* NETWORKTHREADFUNCTIONS_H_ */
