#pragma once
#include "NetworkManager.h"
#include "Player.h"
#include <vector>
#include <memory>

class ClientNetworkManager :
    public NetworkManager
{
public:

    //Send playerStates[playerNum] to server
    void SendPlayerState(std::vector<Ref<Player>> playerStates, int playerNum);

    //Try to receive a playerState from server
    void ReceivePlayerState(std::vector<Ref<Player>> playerStates);

    void SetServerIP(IPaddress server);
};

