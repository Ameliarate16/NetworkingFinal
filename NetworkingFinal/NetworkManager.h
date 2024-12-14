#pragma once
#include "SDL2/SDL_net.h"
#include <vector>
#include <memory>

template<typename T>
using Ref = std::shared_ptr<T>;

class Player;

class NetworkManager
{
public:
	virtual void cleanup();


protected:
	virtual bool InitializeNetworking();

	// Serialize playerStates[playerNum] into packet
	void SerializePlayerState(UDPpacket* packet, std::vector<Ref<Player>> playerStates, int playerNum);

	// Deserialize packet into corresponding playerState
	void DeserializePlayerState(UDPpacket* packet, std::vector<Ref<Player>> playerStates);
};

