#include "VenusFireBall.h"
#include "Animations.h"
#include "Collision.h"
#include <cmath>

CVenusFireBall::CVenusFireBall(float x, float y, float targetX, float targetY) : CGameObject(x, y)
{
	float dx = targetX - x;
	float dy = targetY - y;
	float length = sqrtf(dx * dx + dy * dy);

	if (length <= 0.001f)
	{
		this->vx = -VENUS_FIREBALL_SPEED;
		this->vy = 0.0f;
		this->nx = -1;
	}
	else
	{
		this->vx = dx / length * VENUS_FIREBALL_SPEED;
		this->vy = dy / length * VENUS_FIREBALL_SPEED;
		this->nx = vx >= 0.0f ? 1 : -1;
	}

	spawnTime = GetTickCount64();
}

void CVenusFireBall::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x - VENUS_FIREBALL_BBOX_W / 2.0f;
	top = y - VENUS_FIREBALL_BBOX_H / 2.0f;
	right = x + VENUS_FIREBALL_BBOX_W / 2.0f;
	bottom = y + VENUS_FIREBALL_BBOX_H / 2.0f;
}

void CVenusFireBall::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
}

void CVenusFireBall::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (isDeleted) return;
	if (e->obj->IsBlocking())
		isDeleted = true;
}

void CVenusFireBall::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (GetTickCount64() - spawnTime > VENUS_FIREBALL_LIFETIME)
	{
		isDeleted = true;
		return;
	}

	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CVenusFireBall::Render()
{
	auto ani = CAnimations::GetInstance()->Get(ID_ANI_VENUS_FIREBALL);
	if (ani) ani->Render(x, y, nx);
	//RenderBoundingBox();
}
