// Arrow.h
#ifndef ARROW_H
#define ARROW_H

#include <SDL2/SDL.h>

class Arrow {
public:
    SDL_Rect rect;
    SDL_Color color;
    float velocityX, velocityY;
    bool active;

    Arrow(int x, int y, int size, float dirX, float dirY, SDL_Color color);
    void Update(float deltaTime);
    void Render(SDL_Renderer* renderer);
};

#endif 