#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <iostream>

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
		return 1;
	}

	// on startup, create lobby
	// receive TCP connections from players to populate lobby
	// create a networking thread for each player
	// host player can start game and maybe change settings
	// all players are sent to game map and wait until everyone loads in
	// countdown to game start
	// continually receive and send send player location and cooldown data to all players via UDP
	// when a player dies, mark score or lives or whatever and propagate via TCP
	// once end condition has been reached, declare a winner
	// send players back to lobby

	SDL_Quit();
	return 0;
}