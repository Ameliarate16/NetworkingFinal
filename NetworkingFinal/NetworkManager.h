#pragma once
#include "SDL2/SDL_net.h"
#include <vector>
#include <memory>
#include <mutex>

/*
Contributions:
Name: Amelia Morton, Student Number N01503241 || Implementation of NetworkManager, ClientNetworkManager, and ServerNetworkManager
*/

template<typename T>
using Ref = std::shared_ptr<T>;

class Player;

class NetworkManager
{
public:
	virtual bool InitializeUDP();
	virtual void Cleanup();

	static std::mutex playerStateMutex;


protected:

	// Serialize playerStates[playerNum] into packet
	void SerializePlayerState(UDPpacket* packet, std::vector<Ref<Player>> playerStates, int playerNum);

	// Deserialize packet into corresponding playerState
	void DeserializePlayerState(UDPpacket* packet, std::vector<Ref<Player>> playerStates);

	UDPsocket udpSocket;
	UDPpacket* packet;
};

