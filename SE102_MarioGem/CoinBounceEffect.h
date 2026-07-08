#pragma once

#include "GameObject.h"

constexpr int ID_SPRITE_POINTS_100 = 200037;

class CCoinBounceEffect : public CGameObject
{
	enum class Phase
	{
		CoinBounce,
		PointPopup
	};

	Phase phase;
	float originY;
	ULONGLONG phaseStartTime;

	void StartPointPopup();

public:
	CCoinBounceEffect(float x, float y);

	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
	void Render();
	void GetBoundingBox(float& l, float& t, float& r, float& b);

	int IsBlocking() { return 0; }
	int IsCollidable() { return 0; }
};
