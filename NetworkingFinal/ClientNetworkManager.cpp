#include "ClientNetworkManager.h"
#include "PlayerStateMini.h"


void ClientNetworkManager::SendPlayerState(UDPsocket socket, UDPpacket* packet, std::vector<Ref<Player>> playerStates, int playerNum)
{
	SerializePlayerState(packet, playerStates, playerNum);
	SDLNet_UDP_Send(socket, -1, packet);

	memset(packet->data, 0, packet->maxlen);
}

void ClientNetworkManager::ReceivePlayerState(UDPsocket socket, UDPpacket* packet, std::vector<Ref<Player>> playerStates)
{
	if (SDLNet_UDP_Recv(socket, packet) != 0)
	{
		DeserializePlayerState(packet, playerStates);

		memset(packet->data, 0, packet->maxlen);
	}
}
