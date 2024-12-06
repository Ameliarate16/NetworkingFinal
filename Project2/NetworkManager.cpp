#include "NetworkManager.h"
#include "SDL2/SDL_net.h"

bool NetworkManager::InitializeNetworking()
{
	//initialize SDLNet
	if (SDLNet_Init() < 0) {
		SDL_Log("SDLNet initialization failed: %s", SDLNet_GetError());
		SDL_Quit();
		return false;
	}

    return true;
}
