#include "NetworkManager.h"
#include "SDL2/SDL_net.h"
#include "PlayerStateMini.h"
#include "Player.h"

std::mutex NetworkManager::playerStateMutex;

bool NetworkManager::InitializeUDP()
{
	//open UDP socket
	udpSocket = SDLNet_UDP_Open(8080);
	if (!udpSocket) {
		SDL_Log("SDLNet_UDP_Open error: %s", SDLNet_GetError());
		return false;
	}

	//create UDP packet
	packet = SDLNet_AllocPacket(1024);
	if (!packet) {
		SDL_Log("SDLNet_AllocPacket error: %s", SDLNet_GetError());
		SDLNet_UDP_Close(udpSocket);
		return false;
	}

	return true;
}

void NetworkManager::Cleanup()
{
	SDLNet_FreePacket(packet);
	SDLNet_UDP_Close(udpSocket);
}

void NetworkManager::SerializePlayerState(UDPpacket* packet, std::vector<Ref<Player>> playerStates, int playerNum)
{
	std::lock_guard<std::mutex> lock(playerStateMutex);
	PlayerStateMini playerSerialized(playerNum, *playerStates[playerNum]);
	memcpy(packet->data, &playerSerialized, sizeof(playerSerialized));
	packet->len = sizeof(playerSerialized);

}

void NetworkManager::DeserializePlayerState(UDPpacket* packet, std::vector<Ref<Player>> playerStates)
{

	PlayerStateMini clientSerialized;
	memcpy(&clientSerialized, packet->data, packet->len);

	int playerNum = clientSerialized.playerNum;
	
	std::lock_guard<std::mutex> lock(playerStateMutex);
	playerStates[playerNum]->rect.x = clientSerialized.x;
	playerStates[playerNum]->rect.y = clientSerialized.y;
	playerStates[playerNum]->velX = float(clientSerialized.velXHundredths) / 100.0f;
	playerStates[playerNum]->velY = float(clientSerialized.velYHundredths) / 100.0f;
	playerStates[playerNum]->isDead = clientSerialized.isDead;

	std::chrono::milliseconds lastDash(clientSerialized.dashCooldownMillisecs);
	playerStates[playerNum]->lastDashTime = std::chrono::steady_clock::now() - lastDash;


}