// main.cpp
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <algorithm> // For std::remove_if
#include <cmath>
#include "Player.h"
#include "Arrow.h"
#include "Platform.h"
#include "CollisionChecking.h"

/*
Contributions:
Name: Tan Phi, Student Number N01587949 || Game Implementation, Player, Arrow, Platform, CollisionChecking
*/

// Define screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Collision detection function is now replaced by getCollisionInfo from Collision.cpp

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("TowerFall Clone", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!window || !renderer) {
        std::cerr << "Failed to create window or renderer: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_Color playerColor = { 0, 255, 0, 255 }; // Green
    Player player(375, SCREEN_HEIGHT - 75, 40, playerColor); // Start above the floor

    SDL_Color wallColor = { 255, 255, 255, 255 };
    SDL_Rect wall_1 = { 0, 575, SCREEN_WIDTH , 25 }; // Floor
    SDL_Rect wall_2 = { 0, 0, SCREEN_WIDTH, 25 }; // Ceiling

    // Initialize platforms
    std::vector<Platform> platforms;

    // Static Platform
    platforms.emplace_back(200, 500, 100, 20, SDL_Color{ 0, 0, 255, 255 }); // Blue static platform
    platforms.emplace_back(50, 400, 100, 20, SDL_Color{ 0, 0, 255, 255 }); // Blue static platform
    platforms.emplace_back(400, 450, 100, 20, SDL_Color{ 0, 0, 255, 255 }); // Blue static platform
    platforms.emplace_back(700, 500, 100, 20, SDL_Color{ 0, 0, 255, 255 }); // Blue static platform
    platforms.emplace_back(300, 300, 100, 20, SDL_Color{ 0, 0, 255, 255 }); // Blue static platform

    // Horizontal Moving Platform
    //platforms.emplace_back(500, 300, 100, 20, SDL_Color{ 255, 165, 0, 255 }, PlatformType::LEFT_RIGHT, 100.0f, 150.0f); // Orange horizontal platform
    platforms.emplace_back(300, 200, 100, 20, SDL_Color{ 255, 165, 0, 255 }, PlatformType::LEFT_RIGHT, 100.0f, 150.0f); // Orange horizontal platform

    // Vertical Moving Platform
    platforms.emplace_back(575, 300, 100, 20, SDL_Color{ 255, 0, 255, 255 }, PlatformType::UP_DOWN, 80.0f, 100.0f); // Magenta vertical platform

    // Initialize arrows
    std::vector<Arrow> arrows;

    bool quit = false;
    SDL_Event event;
    Uint32 lastTick = SDL_GetTicks();

    // Frame rate control variables
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;

    Uint32 frameStart;
    int frameTime;

    const float EPSILON = 0.1f;

    while (!quit) {
        frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }

            // Handle key down events
            if (event.type == SDL_KEYDOWN && !event.key.repeat) {
                if (event.key.keysym.sym == SDLK_SPACE) {
                    player.Jump();
                }

                if (event.key.keysym.sym == SDLK_LSHIFT || event.key.keysym.sym == SDLK_RSHIFT) {
                    if (player.canDash) {
                        float dirX = player.velX;
                        float dirY = player.velY;
                        if (dirX == 0 && dirY == 0) {
                            dirX = 1; // Default dash direction to the right
                        }

                        // Normalize dash direction
                        float length = sqrt(dirX * dirX + dirY * dirY);
                        if (length != 0) {
                            dirX /= length;
                            dirY /= length;
                        }

                        player.StartDash(dirX, dirY);
                    }
                }
            }

            // Handle mouse button down event
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                if (!player.isDead) { // Only allow shooting if alive
                    auto now = std::chrono::steady_clock::now();
                    float timeSinceLastShot = std::chrono::duration<float>(now - player.lastShotTime).count();
                    if (timeSinceLastShot >= player.shotCooldown) {
                        int mouseX, mouseY;
                        SDL_GetMouseState(&mouseX, &mouseY);

                        // Calculate direction from player to mouse
                        float dirX = static_cast<float>(mouseX - (player.rect.x + player.rect.w / 2));
                        float dirY = static_cast<float>(mouseY - (player.rect.y + player.rect.h / 2));
                        float length = sqrt(dirX * dirX + dirY * dirY);
                        if (length != 0) {
                            dirX /= length;
                            dirY /= length;
                        }

                        // Determine spawn position based on direction to prevent immediate collision
                        int spawnOffset = 10; // Pixels to offset the arrow spawn
                        int spawnX = player.rect.x + player.rect.w / 2 + static_cast<int>(dirX * (player.rect.w / 2 + spawnOffset));
                        int spawnY = player.rect.y + player.rect.h / 2 + static_cast<int>(dirY * (player.rect.h / 2 + spawnOffset));

                        // Create a new arrow
                        Arrow newArrow(spawnX, spawnY, 20, dirX, dirY, SDL_Color{ 255, 0, 0, 255 });
                        arrows.push_back(newArrow);

                        // Update last shot time
                        player.lastShotTime = now;
                        std::cout << "Arrow shot towards (" << mouseX << ", " << mouseY << ") with direction (" << dirX << ", " << dirY << ")" << std::endl;
                    }
                }
            }
        }

        // Calculate deltaTime
        Uint32 currentTick = SDL_GetTicks();
        float deltaTime = (currentTick - lastTick) / 1000.0f; // seconds
        lastTick = currentTick;

        // Handle input
        const Uint8* keystate = SDL_GetKeyboardState(NULL);
        player.HandleInput(keystate);

        // Store previous Y and X positions
        float previousY = static_cast<float>(player.rect.y);
        float previousX = static_cast<float>(player.rect.x);

        // Update game state
        player.Update(deltaTime);

        // Update platforms
        for (auto& platform : platforms) {
            platform.Update(deltaTime);
        }

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
                player.rect.x = SCREEN_WIDTH;
            }
            else if (player.rect.x > SCREEN_WIDTH) {
                player.rect.x = -player.rect.w;
            }
        }

        // Screen Wrapping - Horizontal (if player is not on a moving platform)
        if (!player.isGrounded) {
            if (player.rect.x + player.rect.w < 0) { // Player moved off the left side
                player.rect.x = SCREEN_WIDTH;
                std::cout << "Player wrapped to the right side of the screen." << std::endl;
            }
            else if (player.rect.x > SCREEN_WIDTH) { // Player moved off the right side
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

        // Rendering
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Clear with black
        SDL_RenderClear(renderer);

        // Render walls
        SDL_SetRenderDrawColor(renderer, wallColor.r, wallColor.g, wallColor.b, wallColor.a);
        SDL_RenderFillRect(renderer, &wall_1);
        SDL_RenderFillRect(renderer, &wall_2);

        // Render platforms
        for (auto& platform : platforms) {
            platform.Render(renderer);
        }

        // Render player
        player.Render(renderer);

        // Render arrows
        for (auto& arrow : arrows) {
            if (arrow.active) {
                arrow.Render(renderer);
            }
        }

        SDL_RenderPresent(renderer);

        // Frame rate control
        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
