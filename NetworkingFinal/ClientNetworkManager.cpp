#include "ClientNetworkManager.h"
#include "PlayerStateMini.h"


void ClientNetworkManager::SendPlayerState(std::vector<Ref<Player>> playerStates, int playerNum)
{
	SerializePlayerState(packet, playerStates, playerNum);
	SDLNet_UDP_Send(udpSocket, -1, packet);

	memset(packet->data, 0, packet->maxlen);
}

void ClientNetworkManager::ReceivePlayerState(std::vector<Ref<Player>> playerStates)
{
	if (SDLNet_UDP_Recv(udpSocket, packet) != 0)
	{
		DeserializePlayerState(packet, playerStates);

		memset(packet->data, 0, packet->maxlen);
	}
}

void ClientNetworkManager::SetServerIP(IPaddress server)
{
	packet->address = server;
}
