#ifndef COLLISION_CHECKING_H
#define COLLISION_CHECKING_H

#include <SDL.h>

enum class CollisionDirection {
    NONE,
    TOP,
    BOTTOM,
    LEFT,
    RIGHT
};

struct CollisionInfo
{
    bool isColliding;
    CollisionDirection direction;
};
    
CollisionInfo checkCollision(const SDL_Rect& a, const SDL_Rect& b);

#endif