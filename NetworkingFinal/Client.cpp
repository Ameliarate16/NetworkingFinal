#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <iostream>
#include <string>
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

	if (SDLNet_Init() < 0)
	{
		std::cerr << "SDL_net initialization failed: " << SDL_GetError() << std::endl;
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
	int serverPort = 8080; 
	std::string username = "Client";

	if (argc > 1)
		serverAddress = argv[1];
	if (argc > 2)
		username = argv[2];
	if (argc > 3)
		serverPort = std::stoi(argv[3]);

	IPaddress ip; 
	if (SDLNet_ResolveHost(&ip, serverAddress.c_str(), serverPort) < 0)
	{
		std::cerr << "Failed to resolve server address: ( " << serverAddress << ":" << serverPort << "): "  << SDLNet_GetError() << std::endl; 
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDLNet_Quit();
		SDL_Quit();
		return 1;
	}

	TCPsocket clientSocket = SDLNet_TCP_Open(&ip);
	if (!clientSocket)
	{
		std::cerr << "Failed to connect to server (" << serverAddress << ":" << serverPort << "): " << SDLNet_GetError() << std::endl;
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDLNet_Quit();
		SDL_Quit();
		return 1;
	}

	std::cout << "Connected to server at" << serverAddress << ":" << serverPort << std::endl;


	// communicate with the server 
	const int BUFFER_SIZE = 512; //will have to change it
	char buffer[BUFFER_SIZE]{0};
	std::string message = "Client has connected to server!";


	if (SDLNet_TCP_Send(clientSocket, message.c_str(), message.length()) < message.length())
	{
		std::cerr << "Failed to send data to server: " << SDLNet_GetError() << std::endl;
	}
	else
	{
		int recived = SDLNet_TCP_Recv(clientSocket, buffer, BUFFER_SIZE - 1);
		if (recived <= 0)
		{
			std::cerr << "Failed to recive data from server: " << SDLNet_GetError() << std::endl; 
		}
		else
		{
			buffer[recived] = '\0';
			std::cout << "Server says: " << buffer << std::endl;
		}
	}

	bool IsRunning = true;
	while (IsRunning) {
		std::cout << "Type 'ready' to start game, or 'exit' to quit game: ";
		std::string input;
		std::getline(std::cin, input);

		if (input == "ready") {
			std::string readyMessage = "Ready";
			if (SDLNet_TCP_Send(clientSocket, readyMessage.c_str(), readyMessage.length()) < readyMessage.length()) {
				std::cerr << "Failed to send READY message to server: " << SDLNet_GetError() << std::endl;
			}
			else 
			{
				std::cout << "Sent READY message to server." << std::endl;
			}
		}
		else if (input == "exit") 
		{
			IsRunning = false;
		}
		else {
			std::cout << "Unknown command. Please type 'ready' or 'exit'." << std::endl;
		}
	}

	SDLNet_TCP_Close(clientSocket);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDLNet_Quit();
	SDL_Quit();
	return 0;
}

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
