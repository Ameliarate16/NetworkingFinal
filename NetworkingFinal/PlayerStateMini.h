#pragma once
#include "Player.h"
#include <chrono>

struct PlayerStateMini
{
	uint8_t playerNum;

	short x;
	short y;
	short velXHundredths;
	short velYHundredths;

	short dashCooldownMillisecs;

	bool isDead;

	PlayerStateMini(int playerNum_, const Player& player_)
	{
		playerNum = playerNum_;
		x = player_.rect.x;
		y = player_.rect.y;
		velXHundredths = player_.velX * 100;
		velYHundredths = player_.velY * 100;

		dashCooldownMillisecs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<float>(std::chrono::steady_clock::now() - player_.lastDashTime)).count();

		isDead = player_.isDead;
	}

	PlayerStateMini() = default;
};