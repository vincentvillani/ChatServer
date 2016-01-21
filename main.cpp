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

#include "AcceptThreadMain.h"
#include "AcceptData.h"
#include "ServerThreadFunctions.h"
#include "AcceptToSeverMailbox.h"



//TODO: Have a way to shutdown the accept thread safely (free it's listening socket)
//TODO: Think about how to delete a Socket* safely: HAVE THE SERVER THREAD WAIT FOR ALL THE WORKER THREADS TO STOP PROCESSING, WAIT FOR THEM ALL TO COMMUNICATE BACK AND THEN DELETE THE SOCKET!
//TODO: Implement non-blocking socket work threads





//SERVER THREAD!
int main()
{
	ServerData serverData;
	AcceptData acceptData;

	AcceptToSeverMailbox acceptToServerMailbox(&serverData, &acceptData);

	//Start the accept thread
	std::thread acceptThread(acceptThreadMain, &acceptData, &acceptToServerMailbox);
	acceptThread.detach();

	//This is the server thread, start running it
	ServerMain(&serverData, &acceptToServerMailbox);


	return 0;
}
