#pragma once
#include "GameObject.h"

constexpr float VENUS_FIREBALL_SPEED = 0.055f;
constexpr int VENUS_FIREBALL_BBOX_W = 8;
constexpr int VENUS_FIREBALL_BBOX_H = 8;
constexpr ULONGLONG VENUS_FIREBALL_LIFETIME = 5000;

constexpr int ID_ANI_VENUS_FIREBALL = 91004;

class CVenusFireBall : public CGameObject
{
protected:
	ULONGLONG spawnTime;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	virtual void OnNoCollision(DWORD dt);
	virtual void OnCollisionWith(LPCOLLISIONEVENT e);

	virtual int IsCollidable() { return 1; }
	virtual int IsBlocking() { return 0; }

public:
	CVenusFireBall(float x, float y, float targetX, float targetY);
};
