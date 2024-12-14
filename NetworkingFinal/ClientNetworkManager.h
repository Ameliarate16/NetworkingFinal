#pragma once
#include "NetworkManager.h"
#include "Player.h"
#include <vector>
#include <memory>

class ClientNetworkManager :
    public NetworkManager
{
public:
    /*
    UDPsocket* udpSocket;
    IPaddress server;
    UDPpacket* packet;
    int numPlayers;
    std::vector<Ref<Player>> playerStates;
    */

    //Send playerStates[playerNum] to server as packet through socket
    void SendPlayerState(UDPsocket socket, UDPpacket* packet, std::vector<Ref<Player>> playerStates, int playerNum);

    //Try to receive a playerState from server as packet through socket
    void ReceivePlayerState(UDPsocket socket, UDPpacket* packet, std::vector<Ref<Player>> playerStates);

};

