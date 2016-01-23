/*
 * main.cpp
 *
 *  Created on: 7 Jan 2016
 *      Author: vincent
 */

#include <stdlib.h>
#include <stdio.h>

#include <thread>


#include "AcceptData.h"
#include "NetworkData.h"

#include "MasterMailbox.h"


#include "ServerThreadFunctions.h"
#include "AcceptThreadMain.h"
#include "NetworkThreadFunctions.h"



//TODO: Have a way to shutdown the network thread safely
//TODO: Think about how to delete a Socket* safely: HAVE THE SERVER THREAD WAIT FOR ALL THE WORKER THREADS TO STOP PROCESSING, WAIT FOR THEM ALL TO COMMUNICATE BACK AND THEN DELETE THE SOCKET!
//TODO: Implement non-blocking socket work threads





//SERVER THREAD!
int main()
{
	ServerData* serverData = new ServerData();
	AcceptData* acceptData = new AcceptData();
	NetworkData* networkData = new NetworkData();

	//AcceptToSeverMailbox acceptToServerMailbox(serverData, acceptData);
	//ServerToNetworkMailbox serverToNetworkMailbox(serverData, networkData);
	MasterMailbox* masterMailbox = new MasterMailbox(serverData, acceptData, networkData);

	//Start the accept thread
	std::thread acceptThread(acceptThreadMain, acceptData, masterMailbox);
	acceptThread.detach();

	//Start the network thread
	std::thread networkThread(NetworkThreadMain, networkData, masterMailbox);
	networkThread.detach();

	//This is the server thread, start running it
	ServerMain(serverData, masterMailbox);

	delete serverData;
	delete acceptData;
	delete networkData;

	delete masterMailbox;

	return 0;
}
