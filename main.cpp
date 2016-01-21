/*
 * main.cpp
 *
 *  Created on: 7 Jan 2016
 *      Author: vincent
 */

#include <stdlib.h>
#include <stdio.h>
//#include <string.h>

#include <thread>
//#include <chrono>


#include "AcceptData.h"
#include "NetworkData.h"

#include "AcceptToSeverMailbox.h"
#include "ServerToNetworkMailbox.h"


#include "ServerThreadFunctions.h"
#include "AcceptThreadMain.h"
#include "NetworkThreadFunctions.h"



//TODO: Have a way to shutdown the network thread safely
//TODO: Think about how to delete a Socket* safely: HAVE THE SERVER THREAD WAIT FOR ALL THE WORKER THREADS TO STOP PROCESSING, WAIT FOR THEM ALL TO COMMUNICATE BACK AND THEN DELETE THE SOCKET!
//TODO: Implement non-blocking socket work threads





//SERVER THREAD!
int main()
{
	ServerData serverData;
	AcceptData acceptData;
	NetworkData networkData;

	AcceptToSeverMailbox acceptToServerMailbox(&serverData, &acceptData);
	ServerToNetworkMailbox serverToNetworkMailbox(&serverData, &networkData);

	//Start the accept thread
	std::thread acceptThread(acceptThreadMain, &acceptData, &acceptToServerMailbox);
	acceptThread.detach();

	//Start the network thread
	std::thread networkThread(NetworkThreadMain, &networkData, &serverToNetworkMailbox);
	networkThread.detach();

	//This is the server thread, start running it
	ServerMain(&serverData, &acceptToServerMailbox, &serverToNetworkMailbox);


	return 0;
}
