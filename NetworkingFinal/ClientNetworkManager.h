#pragma once
#include "NetworkManager.h"
#include "PlayerStateMini.h"
#include <vector>

class ClientNetworkManager :
    public NetworkManager
{
    UDPpacket* networkingPacket;
    std::vector<PlayerStateMini> playerStates;

    void SerializePlayerState(UDPpacket* packet, const PlayerStateMini& ownState);
    void DeserializePlayerStates(UDPpacket* packet, std::vector<PlayerStateMini>& allStates);
};

