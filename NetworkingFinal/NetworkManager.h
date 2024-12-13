#pragma once
#include "SDL2/SDL_net.h"

class NetworkManager
{
public:
	virtual void cleanup();

protected:
	virtual bool InitializeNetworking();
};

