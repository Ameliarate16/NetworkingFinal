#pragma once
#include "NetworkManager.h"
#include "Player.h"
#include <vector>
#include <memory>

class ServerNetworkManager :
	public NetworkManager
{
public: 

	//Send all playerStates to all clients
	void SendPlayerStates(std::vector<Ref<Player>> playerStates, std::vector<IPaddress> clients);
	
	//Receive a playerState from a client
	void ReceivePlayerState(std::vector<Ref<Player>> playerStates);
};

