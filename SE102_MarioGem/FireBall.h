#pragma once
#include "GameObject.h"

// Định nghĩa các hằng số vật lý cho quả cầu lửa
#define FIREBALL_SPEED_X			0.15f	// Tốc độ bay ngang
#define FIREBALL_BOUNCE_SPEED_Y		0.18f	// Lực nảy tưng tưng khi chạm đất
#define FIREBALL_GRAVITY			0.0008f	// Trọng lực kéo quả cầu rơi xuống

#define FIREBALL_BBOX_WIDTH			8
#define FIREBALL_BBOX_HEIGHT		8

#define ID_ANI_FIREBALL				91004	// ID Animation quả cầu bay phải (trong file text)

class CFireBall : public CGameObject
{
protected:
	float ay;				// Gia tốc trọng trường riêng của quả cầu

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual int IsCollidable() { return 1; }
	virtual int IsBlocking() { return 0; } // Cho phép đi xuyên qua Mario và Quái

	virtual void OnNoCollision(DWORD dt);
	virtual void OnCollisionWith(LPCOLLISIONEVENT e);

public:
	CFireBall(float x, float y, int direction);
};