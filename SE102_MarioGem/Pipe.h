#pragma once

#include "GameObject.h"

enum class PipeDirection
{
	Down,
	Up,
	Left,
	Right
};

class CPipe : public CGameObject
{
	float width;
	float height;
	float targetX;
	float targetY;
	PipeDirection entryDirection;
	PipeDirection exitDirection;

public:
	CPipe(float l, float t, float r, float b, float targetX, float targetY,
		PipeDirection entryDirection = PipeDirection::Down,
		PipeDirection exitDirection = PipeDirection::Up);

	void Render();
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void RenderBoundingBox(void);

	float GetTargetX() const { return targetX; }
	float GetTargetY() const { return targetY; }
	PipeDirection GetEntryDirection() const { return entryDirection; }
	PipeDirection GetExitDirection() const { return exitDirection; }

	int IsBlocking() { return 0; }
	int IsCollidable() { return 1; }

	static PipeDirection ParseDirection(const string& direction);
	static LPGAMEOBJECT CreateFromTokens(const vector<string>& tokens);
};
