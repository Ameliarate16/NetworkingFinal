#include "Player.h"
#include <cmath>
#include <iostream>

Player::Player(int x, int y, int size, SDL_Color color)
    : velX(0.0f), velY(0.0f), isDashing(false), canDash(true),
    dashSpeed(1500.0f), dashDuration(0.5f), dashTimeEslaped(0.0f),
    dashCooldown(1.5f), dashDirX(0.0f), dashDirY(0.0f),
    isJumping(false), gravity(1500.0f), jumpSpeed(600.0f), // Adjusted values
    isDead(false), shotCooldown(0.5f), currentPlatform(nullptr),
    isGrounded(false), livesLeft(maxLives)
{
    rect = { x, y, size, size };
    this->color = color;
    lastDashTime = std::chrono::steady_clock::now() - std::chrono::seconds(static_cast<int>(dashCooldown));
    lastShotTime = std::chrono::steady_clock::now() - std::chrono::seconds(static_cast<int>(shotCooldown));
}

void Player::DrawCircleWhenDie(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, centreX + dx, centreY + dy);
            }
        }
    }
}

void Player::HandleInput(const Uint8* keystate)
{
    if (isDashing || isDead) return;

    velX = 0.0f;

    if (keystate[SDL_SCANCODE_D]) velX += 10.0f;
    if (keystate[SDL_SCANCODE_A]) velX -= 10.0f;

    // Uncomment normalization if needed
    /*
    if (velX != 0) {
        float length = sqrt(velX * velX);
        if (length > 0) {
            velX /= length;
        }
    }
    */

    std::cout << "HandleInput - velX: " << velX << ", velY: " << velY << std::endl;
}

void Player::StartDash(float dir_x, float dir_y)
{
    if (canDash && !isDashing) {
        isDashing = true;
        dashDirX = dir_x;
        dashDirY = dir_y;
        canDash = false;
        lastDashTime = std::chrono::steady_clock::now();
        std::cout << "Dash started in direction (" << dashDirX << ", " << dashDirY << ")" << std::endl;
    }
}

void Player::Jump() {
    if (!isJumping && !isDead) {
        velY = -jumpSpeed;
        isJumping = true;
        std::cout << "Jump initiated! velY set to " << velY << std::endl;
    }
}

void Player::Update(float deltaTime)
{
    if (isDead) return;
    float timeSinceLastDash = std::chrono::duration<float>(std::chrono::steady_clock::now() - lastDashTime).count();
    if (!canDash && timeSinceLastDash >= dashCooldown) {
        canDash = true;
    }

    if (isDashing) {
        dashTimeEslaped += deltaTime;
        rect.x += static_cast<int>(dashDirX * dashSpeed * deltaTime);
        rect.y += static_cast<int>(dashDirY * dashSpeed * deltaTime);

        if (dashTimeEslaped >= dashDuration) {
            isDashing = false;
            dashTimeEslaped = 0.0f;
            std::cout << "Dash ended." << std::endl;
        }
    }
    else {
        if (!isGrounded) {
            velY += gravity * deltaTime;

            if (velY > MAX_VEL_Y) {
                velY = MAX_VEL_Y;
            }
        }

        if (velX > MAX_VEL_X) {
            velX = MAX_VEL_X;
        }
        else if (velX < -MAX_VEL_X) {
            velX = -MAX_VEL_X;
        }

        rect.x += static_cast<int>(velX * 100.0f * deltaTime);
        rect.y += static_cast<int>(velY * deltaTime);

        // Floor and ceiling collision handled in main.cpp
    }

    std::cout << "Update - Position: (" << rect.x << ", " << rect.y << "), velX: " << velX << ", velY: " << velY << std::endl;
}

void Player::Render(SDL_Renderer* renderer)
{
    if (isDead) {
        DrawCircleWhenDie(renderer, rect.x + rect.w / 2, rect.y + rect.h / 2, rect.w / 2, color);
    }
    else {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &rect);
    }

    if (!canDash && !isDead) {
        float timeSinceLastDash = std::chrono::duration<float>(std::chrono::steady_clock::now() - lastDashTime).count();
        float cooldownProgress = timeSinceLastDash / dashCooldown;
        if (cooldownProgress > 1.0f)
            cooldownProgress = 1.0f;

        SDL_Rect cooldownBar = { rect.x, rect.y - 10, static_cast<int>(rect.w * cooldownProgress), 5 };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &cooldownBar);
    }
}

//bool Player::WallCollision(SDL_Rect player, SDL_Rect wall)
//{
//    return !(player.x + player.w < wall.x ||
//        player.x > wall.x + wall.w ||
//        player.y + player.h < wall.y ||
//        player.y > wall.y + wall.h);
//}