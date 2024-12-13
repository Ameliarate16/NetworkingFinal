#pragma once

struct PlayerStateMini
{
	uint8_t playerNum;

	short x;
	short y;
	uint8_t velX;
	uint8_t velY;

	short dashCooldownMicrosecs;
	const short dashCooldownMaxMicrosecs;

	bool isJumping;
	bool isDead;
};