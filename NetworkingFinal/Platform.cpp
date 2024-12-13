#include "Platform.h"

Platform::Platform(int x, int y, int width, int height, SDL_Color color, PlatformType platformType, float speed, float range)
	: rect{x, y, width, height}, color(color), platformType(platformType), speed(speed), range(range), direction(1.0f)
{
	if (platformType == PlatformType::LEFT_RIGHT) {
		startPos = static_cast<float>(x);
	}
	else if (platformType == PlatformType::UP_DOWN) {
		startPos = static_cast<float>(y); 
	}
}

void Platform::Update(const float deltaTime)
{
	if (platformType == PlatformType::STATIC) return; 

	if (platformType == PlatformType::LEFT_RIGHT) {
		rect.x += static_cast<int>(speed * direction * deltaTime); 
		if (rect.x > startPos + range || rect.x < startPos - range) {
			direction *= -1.0f; 
		}
	}
	else if (platformType == PlatformType::UP_DOWN) {
		rect.y += static_cast<int>(speed * direction * deltaTime);
		if (rect.y > startPos + range || rect.y < startPos - range) {
			direction *= -1.0f;
		}
	}
}

void Platform::Render(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer, &rect);
}

