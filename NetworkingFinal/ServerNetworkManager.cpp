#include "ServerNetworkManager.h"
#include <iostream>

void ServerNetworkManager::SendPlayerStates(std::vector<Ref<Player>> playerStates, std::vector<IPaddress> clients)
{
	for (int playerNum = 0; playerNum < playerStates.size(); playerNum++)
	{
		SerializePlayerState(packet, playerStates, playerNum);

		for (int clientNum = 0; clientNum < clients.size(); clientNum++)
		{
			packet->address = clients[clientNum];
			if (!SDLNet_UDP_Send(udpSocket, -1, packet))
			{
				std::cerr << "Failed to send player state to client " << clients[clientNum].host 
					<< ":" << clients[clientNum].port << " : " << SDLNet_GetError() << std::endl;
			}

		}

		memset(packet->data, 0, packet->maxlen);
	}
}

void ServerNetworkManager::ReceivePlayerState(std::vector<Ref<Player>> playerStates)
{
	int receiveResult = SDLNet_UDP_Recv(udpSocket, packet);
	if (receiveResult > 0)
	{
		std::cout << "I got something!\n";

		DeserializePlayerState(packet, playerStates);
		
		memset(packet->data, 0, packet->maxlen);
	}
	else if (receiveResult < 0)
	{
		std::cerr << "Error receiving player state from client: " << SDLNet_GetError() << std::endl;
	}
}
