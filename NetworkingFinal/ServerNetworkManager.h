#pragma once
#include "NetworkManager.h"
#include "PlayerStateMini.h"
#include <vector>

class ServerNetworkManager :
	public NetworkManager
{
public: 
	UDPpacket* networkingPacket;
	int numPlayers;
	std::vector<PlayerStateMini> playerStates;

	void SerializePlayerStates(UDPpacket* packet, std::vector<PlayerStateMini>& allStates);
	void DeserializePlayerState(UDPpacket* packet, const PlayerStateMini& clientState);
};

