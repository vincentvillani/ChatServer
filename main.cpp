/*
 * main.cpp
 *
 *  Created on: 7 Jan 2016
 *      Author: vincent
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <thread>
#include <chrono>


#include "ChatServer.h"

int main()
{
	std::chrono::duration<int> tenSeconds = std::chrono::duration<int>(10);

	printf("Sleeping\n");
	std::this_thread::sleep_for(tenSeconds);
	printf("Awake\n");

	ChatServer server;

	server.update();

	return 0;
}
