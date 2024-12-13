#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <chrono>
#include "Platform.h"

class Player
{
public:
	SDL_Rect rect; 
	SDL_Color color; 
	float velX, velY;
	bool isDashing; 
	bool canDash;
	float dashSpeed; 
	float dashDuration; 
	float dashTimeEslaped; 
	std::chrono::steady_clock::time_point lastDashTime; 
	float dashCooldown; 
	std::chrono::steady_clock::time_point lastShotTime;
	float shotCooldown;

	float dashDirX, dashDirY; 

	bool isJumping; 
	float gravity; 
	float jumpSpeed;

	bool isDead; 
	bool isGrounded; 

	const float MAX_VEL_X = 500.0f;
	const float MAX_VEL_Y = 1000.0f; 

	Platform* currentPlatform;

	Player(int x, int y, int size, SDL_Color color);
	void HandleInput(const Uint8* keystate); 
	void StartDash(float dir_x, float dir_y); 
	void Jump(); 
	void Update(const float deltaTime); 
	void Render(SDL_Renderer* renderer); 
	//bool WallCollision(SDL_Rect player, SDL_Rect wall); 
	void DrawCircleWhenDie(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius, SDL_Color color); 
};

#endif 