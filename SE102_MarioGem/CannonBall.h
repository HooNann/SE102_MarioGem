#pragma once
#include "GameObject.h"
#include "AssetIDs.h"
#include <string>

constexpr float     CANNONBALL_SPEED    = 0.15f;
constexpr int       CANNONBALL_BBOX_W   = 16;
constexpr int       CANNONBALL_BBOX_H   = 16;
constexpr ULONGLONG CANNONBALL_LIFETIME = 4000;

class CCannonBall : public CGameObject
{
protected:
	ULONGLONG spawnTime;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	virtual void OnNoCollision(DWORD dt);
	virtual void OnCollisionWith(LPCOLLISIONEVENT e);

	virtual int IsCollidable() { return 1; }
	virtual int IsBlocking()   { return 0; }

public:
	CCannonBall(float x, float y, float vx);
};
