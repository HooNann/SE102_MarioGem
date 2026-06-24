#pragma once
#include "GameObject.h"
#include "AssetIDs.h"
#include <string>

enum class BurnerState : int
{
	Idle   = 0,
	Firing = 1
};

constexpr int BURNER_BBOX_W      = 16;
constexpr int BURNER_BBOX_H_IDLE = 16;
constexpr int BURNER_BBOX_H_FIRE = 48;

constexpr ULONGLONG BURNER_IDLE_TIME = 2000;
constexpr ULONGLONG BURNER_FIRE_TIME = 1600;

constexpr int ID_ANI_BURNER_IDLE = 6000;
constexpr int ID_ANI_BURNER_FIRING = 6001;

class CBurner : public CGameObject
{
protected:
	ULONGLONG stateTimer;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual int IsCollidable() { return state == static_cast<int>(BurnerState::Firing); }
	virtual int IsBlocking()   { return 0; }

public:
	CBurner(float x, float y);
	void SetState(BurnerState state);
	virtual void SetState(int state) override { SetState(static_cast<BurnerState>(state)); }

	static LPGAMEOBJECT CreateFromTokens(const vector<string>& tokens);
};
