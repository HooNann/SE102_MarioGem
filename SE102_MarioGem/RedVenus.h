#pragma once
#include "GameObject.h"
#include <string>

enum class RedVenusState : int
{
	Hidden = 0,
	Rising = 1,
	Ready = 2,
	Lowering = 3
};

constexpr int RED_VENUS_BBOX_W = 16;
constexpr int RED_VENUS_BBOX_H = 32;
constexpr float RED_VENUS_TRAVEL_DISTANCE = 32.0f;
constexpr float RED_VENUS_RISE_SPEED = 0.035f;
constexpr float RED_VENUS_LOWER_SPEED = 0.035f;
constexpr float RED_VENUS_SAFE_DISTANCE = 48.0f;
constexpr ULONGLONG RED_VENUS_READY_TIME = 1500;
constexpr ULONGLONG RED_VENUS_HIDE_TIME = 700;
constexpr ULONGLONG RED_VENUS_SHOOT_COOLDOWN = 2000;
constexpr float RED_VENUS_FIREBALL_OFFSET_Y = -20.0f;

constexpr int ID_ANI_RED_VENUS_HEAD_DOWN = 100013;
constexpr int ID_ANI_RED_VENUS_HEAD_UP = 100015;

class CRedVenus : public CGameObject
{
protected:
	float baseY;
	float topY;
	ULONGLONG stateTimer;
	ULONGLONG lastShotTime;
	bool hasShotInReadyState;

	bool IsMarioFarEnough();
	void UpdateFacingMario();
	int GetHeadAnimationId();
	void ShootAtMario();

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual int IsCollidable() { return state != static_cast<int>(RedVenusState::Hidden); }
	virtual int IsBlocking() { return 0; }

public:
	CRedVenus(float x, float y);
	void SetState(RedVenusState state);
	virtual void SetState(int state) override { SetState(static_cast<RedVenusState>(state)); }

	static LPGAMEOBJECT CreateFromTokens(const vector<string>& tokens);
};
