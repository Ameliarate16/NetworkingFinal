#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <iostream>
#include "ClientNetworkManager.h"

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("Client Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (!window || !renderer) {
		std::cerr << "Failed to create window or renderer: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	std::string serverAddress = "127.0.0.1";
	std::string username = "Client";
	if (argc > 1)
		serverAddress = argv[1];
	if (argc > 2)
		username = argv[2];

	// on startup, connect to lobby via TCP
	// host player can start game and maybe change settings
	// all players are sent to game map and wait until everyone loads in
	// countdown to game start
	// players jump and move and dash-attack to kill each other
	// send player state to server via UDP
	// receive all player states from server via UDP
	// receive game state from server via TCP
	// once end condition has been reached, server declares a winner
	// players sent back to lobby, start over

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}