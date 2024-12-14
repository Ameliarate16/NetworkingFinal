#include "NetworkManager.h"
#include "SDL2/SDL_net.h"
#include "PlayerStateMini.h"
#include "Player.h"

void NetworkManager::cleanup()
{
}

bool NetworkManager::InitializeNetworking()
{
	//initialize SDLNet
	/*if (SDLNet_Init() < 0) {
		SDL_Log("SDLNet initialization failed: %s", SDLNet_GetError());
		SDL_Quit();
		return false;
	}

    return true;*/
	return true;
}

void NetworkManager::SerializePlayerState(UDPpacket* packet, std::vector<Ref<Player>> playerStates, int playerNum)
{
	PlayerStateMini playerSerialized(playerNum, *playerStates[playerNum]);
	memcpy(packet->data, &playerSerialized, sizeof(playerSerialized));
	packet->len = sizeof(playerSerialized);

	/*
	for (auto clientIP : clients)
	{
		packet->address = clientIP;
		SDLNet_UDP_Send(udpSocket, -1, packet);
	}
	*/

}

void NetworkManager::DeserializePlayerState(UDPpacket* packet, std::vector<Ref<Player>> playerStates)
{

	PlayerStateMini clientSerialized;
	memcpy(&clientSerialized, packet->data, packet->len);

	int playerNum = clientSerialized.playerNum;
	
	playerStates[playerNum]->rect.x = clientSerialized.x;
	playerStates[playerNum]->rect.y = clientSerialized.y;
	playerStates[playerNum]->velX = float(clientSerialized.velXHundredths) / 100.0f;
	playerStates[playerNum]->velY = float(clientSerialized.velYHundredths) / 100.0f;
	playerStates[playerNum]->isDead = clientSerialized.isDead;

	std::chrono::milliseconds lastDash(clientSerialized.dashCooldownMillisecs);
	playerStates[playerNum]->lastDashTime = std::chrono::steady_clock::now() - lastDash;


}