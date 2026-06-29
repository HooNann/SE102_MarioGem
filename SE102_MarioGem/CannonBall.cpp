#include "CannonBall.h"
#include "Animations.h"
#include "Collision.h"

CCannonBall::CCannonBall(float x, float y, float vx) : CGameObject(x, y)
{
	this->vx = vx;
	this->vy = 0;
	spawnTime = GetTickCount64();
}

void CCannonBall::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left   = x - CANNONBALL_BBOX_W / 2.0f;
	top    = y - CANNONBALL_BBOX_H / 2.0f;
	right  = x + CANNONBALL_BBOX_W / 2.0f;
	bottom = y + CANNONBALL_BBOX_H / 2.0f;
}

void CCannonBall::OnNoCollision(DWORD dt)
{
	x += vx * dt;
}

void CCannonBall::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (isDeleted) return;
	if (e->obj->IsBlocking())
		isDeleted = true;
}

void CCannonBall::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (GetTickCount64() - spawnTime > CANNONBALL_LIFETIME)
	{
		isDeleted = true;
		return;
	}

	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CCannonBall::Render()
{
	auto ani = CAnimations::GetInstance()->Get(ID_ANI_CANNONBALL);
	if (ani) ani->Render(x, y);
	//RenderBoundingBox();
}
