#include "ServerNetworkManager.h"

void ServerNetworkManager::SendPlayerStates(std::vector<Ref<Player>> playerStates, std::vector<IPaddress> clients)
{
	for (int playerNum = 0; playerNum < playerStates.size(); playerNum++)
	{
		SerializePlayerState(packet, playerStates, playerNum);

		for (int clientNum = 0; clientNum < clients.size(); clientNum++)
		{
			packet->address = clients[clientNum];
			SDLNet_UDP_Send(udpSocket, -1, packet);
		}

		memset(packet->data, 0, packet->maxlen);
	}
}

void ServerNetworkManager::ReceivePlayerState(std::vector<Ref<Player>> playerStates)
{
	if (SDLNet_UDP_Recv(udpSocket, packet) != 0)
	{
		DeserializePlayerState(packet, playerStates);
		
		memset(packet->data, 0, packet->maxlen);
	}
}
