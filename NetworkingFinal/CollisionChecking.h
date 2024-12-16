#ifndef COLLISION_CHECKING_H
#define COLLISION_CHECKING_H

#include <SDL2/SDL.h>

/*
Contributions:
Name: Tan Phi, Student Number N01587949 || Game Implementation, Player, Arrow, Platform, CollisionChecking
*/

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