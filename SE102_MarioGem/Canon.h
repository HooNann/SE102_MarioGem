#pragma once
#include "GameObject.h"
#include "AssetIDs.h"
#include <string>

enum class CanonState : int { Idle = 0, Fired = 1 };

constexpr int       CANON_BBOX_W          = 16;
constexpr int       CANON_BBOX_H          = 32;
constexpr ULONGLONG CANON_FIRE_INTERVAL   = 3000;
constexpr ULONGLONG CANON_FIRED_TIME      = 300;

constexpr int ID_ANI_CANON = 8000;

class CCanon : public CGameObject
{
protected:
	int       direction;
	ULONGLONG stateTimer;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual int IsCollidable() { return 0; }
	virtual int IsBlocking()   { return 0; }

public:
	CCanon(float x, float y, int direction = 1);
	void SetState(CanonState s);
	virtual void SetState(int s) override { SetState(static_cast<CanonState>(s)); }

	static LPGAMEOBJECT CreateFromTokens(const vector<string>& tokens);
};
