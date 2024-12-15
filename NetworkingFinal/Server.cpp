#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <iostream>
//#include <string.h>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include "ServerNetworkManager.h"

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;

std::mutex clientListMutex;
std::vector<bool> playersReady;
//move this later



TCPsocket init(const char* host, Uint16 port)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDLNet_Init();

	IPaddress ip;
	SDLNet_ResolveHost(&ip, host, port);

	TCPsocket server = SDLNet_TCP_Open(&ip);

	return server;
}



void handleClient(TCPsocket client, char* serverMsg, int clientNum, int& numOfClients)
//void handleClient(TCPsocket client)
{
	char buffer[1024];
	//const char* response = "Server received your message.";

	while (true) {
		int received = SDLNet_TCP_Recv(client, buffer, sizeof(buffer));
		if (received > 0) {
			buffer[received - 1] = '\0';  // Null-terminate the received data
			printf("Received: %s\n", buffer);

			// Check for the exit command
			if (strcmp(buffer, "quit") == 0) {
				printf("Client requested to exit. Closing connection.\n");
				break;
			}

			if (strcmp(buffer, "Ready") == 0)
			{
				playersReady[clientNum] = true;
				std::cout << "Player " << clientNum + 1 << " is ready!" << std::endl;
			}

			int readyCounter = 0;
			for (auto readyCheck : playersReady)
			{
				if (readyCheck == true)
				{
					readyCounter++;
					
				}	

			}
			if (numOfClients == readyCounter)
			{
				std::string message = "start";
				SDLNet_TCP_Send(client, message.c_str(), message.length());
			
			}
			


		}
		else {
			// Client disconnected or error occurred
			printf("Client Disconnected\n");
			break;
		}
	}

	SDLNet_TCP_Close(client);

	std::lock_guard<std::mutex> lock(clientListMutex);
	// Handle client removal from your data structures if necessary
}

void networkLoop(bool& serverLoop, TCPsocket server, std::vector<std::thread>& clientThreads, char* serverInput, int& numOfClients)
//void networkLoop(bool serverLoop, TCPsocket server, std::vector<std::thread> &clientThreads, char *serverInput)
{
	while (serverLoop) 
	{

		
		TCPsocket client = SDLNet_TCP_Accept(server);
		if (client) 
		{
			numOfClients++;
			playersReady.push_back(false);
			printf("Client Connected\n");
			// Start a new thread to handle communication with the client
			//std::thread clientThread(handleClient, client);
			std::string message = std::to_string(numOfClients);
			SDLNet_TCP_Send(client, message.c_str(), message.length());
			std::thread clientThread(handleClient, client, serverInput, numOfClients -1, std::ref(numOfClients));
			clientThreads.push_back(std::move(clientThread));


		}

		else
		{
			printf("Searching for connection\n");
			auto currentTime = std::chrono::system_clock::now();

			// Calculate a time point 5 seconds into the future
			auto wakeUpTime = currentTime + std::chrono::seconds(1);

			// Sleep until the specified time
			std::this_thread::sleep_until(wakeUpTime);
		}

		bool everyoneReady = true;
		for (bool playerReady : playersReady)
		{
			if (!playerReady)
			{
				everyoneReady = false;
			}

		}
		if (everyoneReady)
		{
			serverLoop = false;

		}



		
	}

}

void networkGameLoop(bool& serverLoop, TCPsocket server, std::vector<std::thread>& clientThreads, char* serverInput, int& numOfClients)
{
	while (serverLoop)
	{








	}


}



int main(int argc, char* argv[])
{

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
		return 1;
	}
	//Nathan work here

	std::vector<std::thread> clientThreads;

	int numOfClients = 0;
	bool quit = false;
	bool serverLoop = true;
	enum MyEnum
	{

	};

	TCPsocket server = init(NULL, 8080);

	char serverInput[1024];




	while (!quit)
	{
		networkLoop(serverLoop, server, clientThreads, serverInput, numOfClients);


	}




	// on startup, create lobby
	// receive TCP connections from players to populate lobby
	// create a networking thread for each player
	// host player can start game and maybe change settings
	// all players are sent to game map and wait until everyone loads in
	// countdown to game start
	// continually receive and send player location and cooldown data to all players via UDP
	// when a player dies, mark score or lives or whatever and propagate game state via TCP
	// once end condition has been reached, declare a winner
	// send players back to lobby

	SDL_Quit();
	return 0;
}