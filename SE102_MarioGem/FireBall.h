#pragma once
#include "GameObject.h"

#define FIREBALL_SPEED_X			0.15f
#define FIREBALL_BOUNCE_SPEED_Y		0.18f
#define FIREBALL_GRAVITY			0.0008f

#define FIREBALL_BBOX_WIDTH			8
#define FIREBALL_BBOX_HEIGHT		8

#define ID_ANI_FIREBALL				91004

class CFireBall : public CGameObject
{
protected:
	float ay;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual int IsCollidable() { return !IsDeleted(); }
	virtual int IsBlocking() { return 0; }

	virtual void OnNoCollision(DWORD dt);
	virtual void OnCollisionWith(LPCOLLISIONEVENT e);

public:
	CFireBall(float x, float y, int direction);
};
