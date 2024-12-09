// Arrow.cpp
#include "Arrow.h"

Arrow::Arrow(int x, int y, int size, float dirX, float dirY, SDL_Color color)
    : velocityX(dirX * 500.0f), velocityY(dirY * 500.0f), color(color), active(true)
{
    rect = { x, y, size, size / 4 }; // Arrow shape (e.g., rectangle)
}

void Arrow::Update(float deltaTime) {
    rect.x += static_cast<int>(velocityX * deltaTime);
    rect.y += static_cast<int>(velocityY * deltaTime);

    // Deactivate arrow if it goes off-screen
    if (rect.x < 0 || rect.x > 800 || rect.y < 0 || rect.y > 600) {
        active = false;
    }
}

void Arrow::Render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}