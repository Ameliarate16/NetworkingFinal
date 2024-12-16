#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <iostream>
//#include <string.h>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include "ServerNetworkManager.h"
#include "Arrow.h"
#include "Platform.h"
#include "Player.h"
#include "CollisionChecking.h"

/*
Contributions:
Name: Nathan Brobbey, Student Number N01588517 || TCP Lobby
Name: Amelia Morton, Student Number N01503241 || UDP, Game Loop (game logic copied from Tan's test.cpp), and bug fixing
*/

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

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
	
	const SDL_Color playerColor[4] = { 
		{255, 0, 0, 255},
		{0, 0, 255, 255},
		{0, 255, 0, 255},
		{255, 255, 0, 255}
	};

	const std::pair<int, int> playerStartingPositions[4] = {
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
	
	const SDL_Color WALL_COLOR = { 255, 255, 255, 255 };

	SDL_Rect wall_1 = { 0, 575, WINDOW_WIDTH , 25 }; // Floor
	SDL_Rect wall_2 = { 0, 0, WINDOW_WIDTH, 25 }; // Ceiling

	// Initialize platforms
	std::vector<Platform> platforms;

	// Static Platform
	platforms.emplace_back(200, 500, 100, 20, WALL_COLOR); // static platform
	platforms.emplace_back(50, 400, 100, 20, WALL_COLOR); // static platform
	platforms.emplace_back(400, 450, 100, 20, WALL_COLOR); // static platform
	platforms.emplace_back(700, 500, 100, 20, WALL_COLOR); // static platform
	platforms.emplace_back(300, 300, 100, 20, WALL_COLOR); // static platform

	// Initialize arrows
	std::vector<Arrow> arrows;

	Uint32 lastTick = SDL_GetTicks();

	// Frame rate control variables
	const int FPS = 60;
	const int frameDelay = 1000 / FPS;

	Uint32 frameStart;
	int frameTime;

	const float EPSILON = 0.1f;

	std::thread udpThread = std::thread(handleUDP, 
		std::ref(serverLoop), std::ref(serverUDP), std::ref(playerStates), 
		std::ref(disconnectedClients), std::ref(numOfClients), std::ref(clientIPs));

	while (serverLoop)
	{
		// Calculate deltaTime
		Uint32 currentTick = SDL_GetTicks();
		float deltaTime = (currentTick - lastTick) / 1000.0f; // seconds
		lastTick = currentTick;

		NetworkManager::playerStateMutex.lock();
		for (auto player : playerStates)
		{
			player->Update(deltaTime);
		}
		NetworkManager::playerStateMutex.unlock();

		// Update arrows
		for (auto& arrow : arrows) {
			if (arrow.active) {
				arrow.Update(deltaTime);
			}
		}

		// Remove inactive arrows
		arrows.erase(std::remove_if(arrows.begin(), arrows.end(),
			[](const Arrow& a) { return !a.active; }), arrows.end());

		// Conditional Collision Handling

		NetworkManager::playerStateMutex.lock();
		for (auto playerState : playerStates)
		{
			Player player = *playerState;

			// Reset currentPlatform pointer
			player.currentPlatform = nullptr;

			// Collision with floor
			CollisionInfo collision = checkCollision(player.rect, wall_1);
			if (collision.isColliding && collision.direction == CollisionDirection::BOTTOM) {
				player.rect.y = wall_1.y - player.rect.h - static_cast<int>(EPSILON);
				player.velY = 0.0f;
				player.isJumping = false;
				player.isGrounded = true;
				std::cout << "Player landed on the floor." << std::endl;
			}
			else {
				player.isGrounded = false;
			}

			// Collision with ceiling
			collision = checkCollision(player.rect, wall_2);
			if (collision.isColliding && collision.direction == CollisionDirection::TOP) {
				player.rect.y = wall_2.y + wall_2.h + static_cast<int>(EPSILON);
				player.velY = 0.0f;
				std::cout << "Player hit the ceiling." << std::endl;
			}

			// Collision with platforms
			for (auto& platform : platforms) {
				collision = checkCollision(player.rect, platform.rect);
				if (collision.isColliding) {
					switch (collision.direction) {
					case CollisionDirection::TOP:
						// Player hits the bottom of a platform
						player.rect.y = platform.rect.y + platform.rect.h;
						player.velY = 0.0f;
						std::cout << "Player hit the bottom of a platform." << std::endl;
						break;
					case CollisionDirection::BOTTOM:
						// Player lands on top of a platform
						player.rect.y = platform.rect.y - player.rect.h;
						player.velY = 0.0f;
						player.isJumping = false;
						player.isGrounded = true;
						player.currentPlatform = &platform;
						std::cout << "Player landed on a platform." << std::endl;
						break;
					case CollisionDirection::LEFT:
						// Player hits the left side of a platform
						player.rect.x = platform.rect.x - player.rect.w;
						player.velX = 0.0f;
						std::cout << "Player hit the left side of a platform." << std::endl;
						break;
					case CollisionDirection::RIGHT:
						// Player hits the right side of a platform
						player.rect.x = platform.rect.x + platform.rect.w;
						player.velX = 0.0f;
						std::cout << "Player hit the right side of a platform." << std::endl;
						break;
					default:
						break;
					}
				}
			}


			// Move the player along with the platform if standing on one
			if (player.isGrounded && player.currentPlatform != nullptr && player.velY == 0.0f) {
				if (player.currentPlatform->platformType == PlatformType::LEFT_RIGHT) {
					player.rect.x += static_cast<int>(player.currentPlatform->speed * player.currentPlatform->direction * deltaTime);
				}
				else if (player.currentPlatform->platformType == PlatformType::UP_DOWN) {
					player.rect.y += static_cast<int>(player.currentPlatform->speed * player.currentPlatform->direction * deltaTime);
				}

				// Ensure the player doesn't move beyond screen boundaries when moving with the platform
				if (player.rect.x + player.rect.w < 0) {
					player.rect.x = WINDOW_WIDTH;
				}
				else if (player.rect.x > WINDOW_WIDTH) {
					player.rect.x = -player.rect.w;
				}
			}

			// Screen Wrapping - Horizontal (if player is not on a moving platform)
			if (!player.isGrounded) {
				if (player.rect.x + player.rect.w < 0) { // Player moved off the left side
					player.rect.x = WINDOW_WIDTH;
					std::cout << "Player wrapped to the right side of the screen." << std::endl;
				}
				else if (player.rect.x > WINDOW_WIDTH) { // Player moved off the right side
					player.rect.x = -player.rect.w;
					std::cout << "Player wrapped to the left side of the screen." << std::endl;
				}
			}

			// Check collisions between arrows and player
			for (auto& arrow : arrows) {
				if (!arrow.active) continue;

				CollisionInfo checkClollision = checkCollision(arrow.rect, player.rect);

				if (checkClollision.isColliding) {
					arrow.active = false;
					// Handle player death
					player.isDead = true;
					player.velX = 0;
					player.velY = 0;
					player.currentPlatform = nullptr; // Reset current platform
					std::cout << "Player hit by an arrow!" << std::endl;
				}
			}
		}
		NetworkManager::playerStateMutex.unlock();
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


	
	networkLobbyLoop(serverLoop, server, clientThreads, serverInput, numOfClients, disconnectedClients);
	serverLoop = true;
	networkGameLoop(serverLoop, server, clientThreads, serverInput, numOfClients, disconnectedClients, clientIPs, playerStates);




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