/*
 * ChatMessage.h
 *
 *  Created on: 14 Jan 2016
 *      Author: vincentvillani
 */

#ifndef CHATMESSAGE_H_
#define CHATMESSAGE_H_

#include <string>

class ChatMessage
{

public:

	ChatMessage();
	virtual ~ChatMessage();

	std::string userName;
	std::string chatMessage;
};

#endif /* CHATMESSAGE_H_ */
