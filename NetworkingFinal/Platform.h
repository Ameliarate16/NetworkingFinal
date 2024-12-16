#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL2/SDL.h>

/*
Contributions:
Name: Tan Phi, Student Number N01587949 || Game Implementation, Player, Arrow, Platform, CollisionChecking
*/

enum class PlatformType {
	STATIC,
	UP_DOWN,
	LEFT_RIGHT
};

class Platform
{
public:
	SDL_Rect rect; 
	SDL_Color color; 
	PlatformType platformType; 
	float speed; 
	float range;
	float direction; 
	float startPos; 

	Platform(int x, int y, int width, int height, SDL_Color color, PlatformType platformType = PlatformType::STATIC, float speed = 0.0f, float range = 0.0f); 

	void Update(const float deltaTime); 
	void Render(SDL_Renderer* renderer);  
};

#endif