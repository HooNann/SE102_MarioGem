#pragma once
#include "GameObject.h"
#include "AssetIDs.h"
#include <string>

enum class BlasterState : int { Idle = 0, Firing = 1 };

constexpr int BLASTER_BBOX_W       = 12;
constexpr int BLASTER_BBOX_H       = 48;
constexpr int BLASTER_BLAST_HEIGHT = 48;
constexpr ULONGLONG BLASTER_IDLE_TIME = 2000;
constexpr ULONGLONG BLASTER_FIRE_TIME = 1500;

constexpr int ID_ANI_BLASTER_IDLE = 7000;
constexpr int ID_ANI_BLASTER_FIRING = 7001;

class CBlaster : public CGameObject
{
protected:
	ULONGLONG stateTimer;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual int IsCollidable() { return state == static_cast<int>(BlasterState::Firing); }
	virtual int IsBlocking()   { return 0; }

public:
	CBlaster(float x, float y);
	void SetState(BlasterState state);
	virtual void SetState(int state) override { SetState(static_cast<BlasterState>(state)); }

	static LPGAMEOBJECT CreateFromTokens(const vector<string>& tokens);
};
