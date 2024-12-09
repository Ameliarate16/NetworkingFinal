#include "CollisionChecking.h"

CollisionInfo checkCollision(const SDL_Rect& a, const SDL_Rect& b)
{
    CollisionInfo check = { false, CollisionDirection::NONE };

	int leftA = a.x; 
	int rightA = a.x + a.w; 
	int topA = a.y;
	int bottomA = a.y + a.h;

	int leftB = b.x;
	int rightB = b.x + b.w;
	int topB = b.y;
	int bottomB = b.y + b.h;

	if (rightA <= leftB || leftA >= rightB || bottomA <= topB || topA >= bottomB) {
		return check; // No collision
	}

    // calculate the distance, consider A to B
    int leftDistance = rightA - leftB;
    int rightDistance = rightB - leftA;
    int bottomDistance = bottomA - topB;
    int topDistance = bottomB - topA;

    // assume the collision likely to happen
    int minDistance = leftDistance;
    CollisionDirection dir = CollisionDirection::LEFT;

    if (rightDistance < minDistance) {
        minDistance = rightDistance;
        dir = CollisionDirection::RIGHT;
    }
    if (topDistance < minDistance) {
        minDistance = topDistance;
        dir = CollisionDirection::TOP;
    }
    if (bottomDistance < minDistance) {
        minDistance = bottomDistance;
        dir = CollisionDirection::BOTTOM;
    }

    check.isColliding = true;
    check.direction = dir;
    return check;
}
