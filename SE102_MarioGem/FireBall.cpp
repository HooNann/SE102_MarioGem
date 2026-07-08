#include "FireBall.h"
#include "Goomba.h"
#include "Koopas.h"
#include "BoomBoom.h"
#include "RedVenus.h"
#include "Mario.h"

CFireBall::CFireBall(float x, float y, int direction) : CGameObject(x, y)
{
	this->nx = direction;
	this->vx = direction * FIREBALL_SPEED_X;
	this->vy = 0;
	this->ay = FIREBALL_GRAVITY;
}

void CFireBall::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt;

	if (coObjects != NULL)
	{
		float l, t, r, b;
		GetBoundingBox(l, t, r, b);

		for (size_t i = 0; i < coObjects->size(); i++)
		{
			LPGAMEOBJECT obj = coObjects->at(i);
			if (dynamic_cast<CMario*>(obj)) continue;
			if (obj->IsDeleted() || !obj->IsBlocking()) continue;
			if (obj->IsDirectionColliable((float)-nx, 0) != 1) continue;

			float sl, st, sr, sb;
			obj->GetBoundingBox(sl, st, sr, sb);

			if (l < sr && r > sl && t < sb && b > st)
			{
				this->Delete();
				return;
			}
		}
	}

	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CFireBall::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
}

void CFireBall::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (dynamic_cast<CMario*>(e->obj)) return;

	if (e->obj->IsBlocking())
	{
		if (e->ny < 0)
		{
			vy = -FIREBALL_BOUNCE_SPEED_Y;
		}
		else if (e->nx != 0)
		{
			this->Delete();
		}
	}

	if (dynamic_cast<CGoomba*>(e->obj))
	{
		CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);

		// Fireball knocks Goomba out instead of flattening it like a stomp.
		goomba->SetState(GoombaState::Die_KnockOut);

		this->Delete();
	}

	if (dynamic_cast<CKoopas*>(e->obj))
	{
		CKoopas* koopas = dynamic_cast<CKoopas*>(e->obj);
		koopas->SetState(KoopasState::Die_KnockOut);
		this->Delete();
	}

	if (dynamic_cast<CBoomBoom*>(e->obj))
	{
		CBoomBoom* boomBoom = dynamic_cast<CBoomBoom*>(e->obj);
		boomBoom->TakeDamage();
		this->Delete();
	}

	if (dynamic_cast<CRedVenus*>(e->obj))
	{
		CRedVenus* redVenus = dynamic_cast<CRedVenus*>(e->obj);
		redVenus->Delete();
		this->Delete();
	}
}

void CFireBall::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	LPANIMATION ani = animations->Get(ID_ANI_FIREBALL);
	if (ani != NULL)
		ani->Render(x, y, nx);
}

void CFireBall::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x - FIREBALL_BBOX_WIDTH / 2;
	top = y - FIREBALL_BBOX_HEIGHT / 2;
	right = left + FIREBALL_BBOX_WIDTH;
	bottom = top + FIREBALL_BBOX_HEIGHT;
}
