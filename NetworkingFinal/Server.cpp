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



void handleClient(TCPsocket client, char* serverMsg, int clientNum, int& numOfClients, std::vector<bool>& disconnectedClients)
//void handleClient(TCPsocket client)
{
	char buffer[1024];
	//const char* response = "Server received your message.";

	bool gameStarted = false;

	SDLNet_SocketSet set = SDLNet_AllocSocketSet(1);
	SDLNet_TCP_AddSocket(set, client);

	while (true) {
		if (!gameStarted)
		{
			int socketCheck = SDLNet_CheckSockets(set, 0);
			if (socketCheck > 0)
			{

				int received = SDLNet_TCP_Recv(client, buffer, sizeof(buffer));
				if (received > 0) {
					buffer[received] = '\0';  // Null-terminate the received data
					printf("Received: %s\n", buffer);

					// Check for the exit command
					if (strcmp(buffer, "quit") == 0) {
						printf("Client requested to exit. Closing connection.\n");
						break;
					}

					if (strcmp(buffer, "Ready") == 0)
					{
						std::lock_guard<std::mutex> lock(clientListMutex);
						playersReady[clientNum] = true;
						std::cout << "Player " << clientNum + 1 << " is ready!" << std::endl;
					}

			


				}
				else {
					// Client disconnected 
					printf("Client Disconnected\n");
					break;
				}

			}
			else if (socketCheck < 0)
			{
				std::cerr << "Error checking client socket: " << SDLNet_GetError() << std::endl;
				break;
			}

			int readyCounter = 0;
			for (auto readyCheck : playersReady)
			{
				if (readyCheck == true || disconnectedClients[readyCounter])
				{
					readyCounter++;
					
				}	

			}
			if (numOfClients == readyCounter)
			{
				std::string message = "start";
				SDLNet_TCP_Send(client, message.c_str(), message.length());
				gameStarted = true;
			}
		}

		else if (gameStarted)
		{
			int socketCheck = SDLNet_CheckSockets(set, 0);
			if (socketCheck > 0)
			{
				int received = SDLNet_TCP_Recv(client, buffer, sizeof(buffer));
				if (received > 0) {
				}
				else {
					// Client disconnected 
					printf("Client Disconnected\n");
					break;
				}
			}
			else if (socketCheck < 0)
			{
				std::cerr << "Error checking client socket: " << SDLNet_GetError() << std::endl;
				break;
			}

		}
	}

	SDLNet_TCP_Close(client);

	std::lock_guard<std::mutex> lock(clientListMutex);
	disconnectedClients[clientNum] = true;
	// Handle client removal from your data structures if necessary
}

void handleUDP(bool& gameLoop, ServerNetworkManager& serverUDP, std::vector<Ref<Player>>& playerStates, std::vector<bool>& disconnectedClients, const int& numOfClients, const std::vector<IPaddress>& clientIPs)
{
	while (gameLoop)
	{
		int numDisconnected = 0;
		for (auto clientDisconnected : disconnectedClients)
		{
			if (clientDisconnected)
			{
				numDisconnected++;
			}
		}
		for (int i = 0; i < numOfClients - numDisconnected; i++)
		{
			serverUDP.ReceivePlayerState(playerStates);
		}

		serverUDP.SendPlayerStates(playerStates, clientIPs);

		SDL_Delay(16);
	}
}


void networkLobbyLoop(bool& serverLoop, TCPsocket server, std::vector<std::thread>& clientThreads, char* serverInput, int& numOfClients, std::vector<bool>& disconnectedClients)
//void networkLobbyLoop(bool serverLoop, TCPsocket server, std::vector<std::thread> &clientThreads, char *serverInput)
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
			disconnectedClients.push_back(false);
			std::thread clientThread(handleClient, client, serverInput, numOfClients -1, std::ref(numOfClients), std::ref(disconnectedClients));
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
		if (everyoneReady && numOfClients > 1)
		{
			serverLoop = false;

		}



		
	}

}

void networkGameLoop(bool& serverLoop, TCPsocket server, std::vector<std::thread>& clientThreads, char* serverInput, int& numOfClients, std::vector<bool>& disconnectedClients, std::vector<IPaddress> clientIPs, std::vector<Ref<Player>> playerStates)
{
	
	ServerNetworkManager serverUDP;
	serverUDP.InitializeUDP();
	
	SDL_Color playerColor[4] = { 
		{255, 0, 0, 255},
		{0, 0, 255, 255},
		{0, 255, 0, 255},
		{255, 255, 0, 255}
	};

	std::pair<int, int> playerStartingPositions[4] = {
		{375, WINDOW_HEIGHT - 75},
		{375, WINDOW_HEIGHT - 75},
		{375, WINDOW_HEIGHT - 75},
		{375, WINDOW_HEIGHT - 75}
	};

	for (int i = 0; i < numOfClients; i++)
	{
		playerStates.push_back(
			std::make_shared<Player>(
				playerStartingPositions[i].first, playerStartingPositions[i].second, 40, playerColor[i]
			)
		);
	}
	
	std::thread udpThread = std::thread(handleUDP, 
		std::ref(serverLoop), std::ref(serverUDP), std::ref(playerStates), 
		std::ref(disconnectedClients), std::ref(numOfClients), std::ref(clientIPs));

	while (serverLoop)
	{

	}


	serverUDP.Cleanup();
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
	std::vector<bool> disconnectedClients;

	int numOfClients = 0;
	bool quit = false;
	bool serverLoop = true;
	enum MyEnum
	{

	};

	TCPsocket server = init(NULL, 8080);

	char serverInput[1024];

	std::vector<IPaddress> clientIPs;
	std::vector<Ref<Player>> playerStates;

	
	while (!quit)
	{
		networkLobbyLoop(serverLoop, server, clientThreads, serverInput, numOfClients, disconnectedClients);
		serverLoop = true;
		networkGameLoop(serverLoop, server, clientThreads, serverInput, numOfClients, disconnectedClients, clientIPs, playerStates);

	}



	for (int i = 0; i < clientThreads.size(); i++)
	{
		clientThreads[i].join();
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