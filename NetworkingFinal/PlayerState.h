#pragma once

struct PlayerState
{
	int x;
	int y;

	float dashCooldown;
	const float dashCooldownMax;

	bool isGrounded;
};