#pragma once
#include "NetworkManager.h"
#include "Player.h"
#include <vector>
#include <memory>

class ServerNetworkManager :
	public NetworkManager
{
public: 
	/*
	UDPsocket udpSocket;
	std::vector<IPaddress> clients;
	UDPpacket* packet;
	int numPlayers;
	std::vector<Ref<Player>> playerStates;
	*/

	//Send all playerStates to all clients as packet through socket
	void SendPlayerStates(UDPsocket socket, UDPpacket* packet, std::vector<Ref<Player>> playerStates, std::vector<IPaddress> clients);
	
	//Receive a playerState from a client as packet through socket
	void ReceivePlayerState(UDPsocket socket, UDPpacket* packet, std::vector<Ref<Player>> playerStates);
};

