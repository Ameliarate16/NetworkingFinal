#pragma once
#include "NetworkManager.h"
#include "Player.h"
#include <vector>
#include <memory>

/*
Contributions:
Name: Amelia Morton, Student Number N01503241 || Implementation of NetworkManager, ClientNetworkManager, and ServerNetworkManager
*/

class ServerNetworkManager :
	public NetworkManager
{
public: 

	//Send all playerStates to all clients
	void SendPlayerStates(std::vector<Ref<Player>> playerStates, std::vector<IPaddress> clients);
	
	//Receive a playerState from a client
	void ReceivePlayerState(std::vector<Ref<Player>> playerStates);
};

