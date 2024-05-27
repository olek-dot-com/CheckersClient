#include "NetworkClient.h"

#include <iostream>


enum class CustomMsgTypes : uint8_t
{
	ServerAccept,
	ServerDeny,
	ServerPing,
	MessageAll,
	ServerMessage,
}; 

class CustomClient : public olc::net::NetworkClient<CustomMsgTypes>
{
public:
	void pingServer(int isWhiteMove, int fromCol, int fromRow, int toRow, int toCol) {
		//olc::net::Message<CustomMsgTypes> msg;
		//msg.header.id = CustomMsgTypes::ServerPing;
		//Caution
		//std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
		//msg << timeNow;
		olc::net::Message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::ServerPing;
		int a = (uint8_t)toCol, b = (uint8_t)toRow, c = (uint8_t)fromCol, d = (uint8_t)fromRow, e = (uint8_t)isWhiteMove;
		msg << a << b << c << d << e;
		send(msg);
	}

	void messageAll(int isWhiteMove, int fromCol, int fromRow, int toRow, int toCol) {
		olc::net::Message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::MessageAll;
		//std::cout << "sending message" << " from col " << fromCol << "from row " << fromRow << "to col " << toCol << "toRow" << toRow << std::endl;
		//std::cout << "Uint8_t from col " << (uint8_t)fromCol << "Uint8_t from row " << (uint8_t)fromRow << "Uint8_t to col " << (uint8_t)toCol << "Uint8_t to row " << (uint8_t)toRow << std::endl;
		//uint8_t a = (uint8_t)2, b = (uint8_t)3, c = (uint8_t)4, d = (uint8_t)5;
		//uint8_t a = (uint8_t)toCol, b = (uint8_t)toRow, c = (uint8_t)fromCol, d = (uint8_t)fromRow, e = (uint8_t)isWhiteMove;
		//msg << a << b /*<< c << d << e*/;
		msg << (uint8_t)toCol << (uint8_t)toRow << (uint8_t)fromCol << (uint8_t)fromRow << (uint8_t) isWhiteMove;
		send(msg);
		//std::cout << "Message sent to all clients" << std::endl;
	}

};
