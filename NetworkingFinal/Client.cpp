#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <iostream>
#include <string>
#include "ClientNetworkManager.h"

/*
Contributions:
Name: Matteo Mastrantoni, Student Number N01541856 || Client.cpp code
Name: Joseph D'Elia, n01438727 || implementing the level/player data into Client.cpp 
*/

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600; 

//game colors
const SDL_Color PLAYER_COLOR = { 0, 255, 0, 255 };
const SDL_Color WALL_COLOR = { 255, 255, 255, 255 };

void RunGame(SDL_Renderer* renderer, TCPsocket clientSocket, int playerNumber);
void HandleGameCommunication(TCPsocket clientSocket, bool& isRunning, std::vector<std::string>& serverMessages);

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
	
	//implent a TCP recive function for what player number you are 
	// communicate with the server 
	const int BUFFER_SIZE = 512; //will have to change it
	char buffer[BUFFER_SIZE]{0};

	int recived = SDLNet_TCP_Recv(clientSocket, buffer, BUFFER_SIZE - 1);
	if(recived <= 0)
	{
		std::cerr << "Failed to recivee data from server: " << SDLNet_GetError() << std::endl; 
		SDLNet_TCP_Close(clientSocket);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDLNet_Quit();
		SDL_Quit();
		return 1;
	}

	buffer[recived] = '\0';
	std::string NumberPlayerStr(buffer); 
	std::cout << "Assigning the Player Number from the server: " << NumberPlayerStr << std::endl;
	
	int NumberOfPlayer = std::stoi(NumberPlayerStr);
	std::cout << "Player number stored as: " << NumberOfPlayer << std::endl;

	ClientNetworkManager clientUDP;

	//recive a message of start from the server when the game is active.  
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

		//recive the meesage from the server 
		recived = SDLNet_TCP_Recv(clientSocket, buffer, BUFFER_SIZE - 1);
		if (recived > 0)
		{
			buffer[recived] = '\0';
			std::cout << " Server Message says: " << buffer << std::endl;
			break;
		}
		else if (recived == 0)
		{
			std::cerr << "Server has been Disconnected" << std::endl; 
			IsRunning = false;
		}
	}

	RunGame(renderer, clientSocket, NumberOfPlayer);

	SDLNet_TCP_Close(clientSocket);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDLNet_Quit();
	SDL_Quit();
	return 0;
}

//main game loop
void RunGame(SDL_Renderer* renderer, TCPsocket clientSocket, int playerNumber)
{
	bool isRunning = true;
	SDL_Event event;
	Uint32 lastTick = SDL_GetTicks();
	std::vector<std::string> serverMessages;

	while (isRunning)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				isRunning = false;
			}
		}


		//clear screen
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		//render player
		SDL_Rect playerRect = { 100 + (playerNumber * 50), 300, 50, 50 };
		SDL_SetRenderDrawColor(renderer, PLAYER_COLOR.r, PLAYER_COLOR.g, PLAYER_COLOR.b, PLAYER_COLOR.a);
		SDL_RenderFillRect(renderer, &playerRect);

		//render floor
		SDL_Rect floorRect = { 0, WINDOW_HEIGHT - 50, WINDOW_WIDTH, 50 };
		SDL_SetRenderDrawColor(renderer, WALL_COLOR.r, WALL_COLOR.g, WALL_COLOR.b, WALL_COLOR.a);
		SDL_RenderFillRect(renderer, &floorRect);

		//present the rendered frame
		SDL_RenderPresent(renderer);

		//handle server communication
		HandleGameCommunication(clientSocket, isRunning, serverMessages);

		//frame delay
		Uint32 currentTick = SDL_GetTicks();
		if (currentTick - lastTick < 16)
		{
			SDL_Delay(16 - (currentTick - lastTick));
		}
		lastTick = currentTick;

	}
}

//handle communication with the server
void HandleGameCommunication(TCPsocket clientSocket, bool& isRunning, std::vector<std::string>& serverMessages)
{
	const int BUFFER_SIZE = 512;
	char buffer[BUFFER_SIZE] = { 0 };

	//receive server messages
	int received = SDLNet_TCP_Recv(clientSocket, buffer, BUFFER_SIZE - 1);
	if (received > 0)
	{
		buffer[received] = '\0';
		serverMessages.push_back(std::string(buffer));
		std::cout << "Server: " << buffer << std::endl;
	}
	else if (received <= 0)
	{
		std::cerr << "Disconnected from server." << std::endl;
		isRunning = false;
	}
	
	std::string positionMessage = "Player" + std::to_string(SDL_GetTicks() % 100);
	SDLNet_TCP_Send(clientSocket, positionMessage.c_str(), positionMessage.length());
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
