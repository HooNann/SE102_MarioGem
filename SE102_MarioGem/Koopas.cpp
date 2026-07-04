#include "Koopas.h"
#include "Goomba.h"

namespace
{
	constexpr int ToInt(KoopasState state)
	{
		return static_cast<int>(state);
	}

	int DirectionFromVelocity(float vx)
	{
		return vx < 0 ? -1 : 1;
	}
}

CKoopas::CKoopas(float x, float y) : CGameObject(x, y)
{
	ax = 0;
	ay = KOOPAS_GRAVITY;
	SetState(KoopasState::Walking);
}

void CKoopas::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	int height = IsWalking() ? KOOPAS_BBOX_HEIGHT : KOOPAS_SHELL_BBOX_HEIGHT;

	left = x - KOOPAS_BBOX_WIDTH / 2;
	top = y - height / 2;
	right = left + KOOPAS_BBOX_WIDTH;
	bottom = top + height;
}

void CKoopas::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
}

void CKoopas::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (IsShellMoving())
	{
		if (dynamic_cast<CGoomba*>(e->obj))
		{
			CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
			goomba->SetState(GoombaState::Die_KnockOut);
			return;
		}

		if (dynamic_cast<CKoopas*>(e->obj))
		{
			CKoopas* koopas = dynamic_cast<CKoopas*>(e->obj);
			if (koopas != this)
				koopas->SetState(KoopasState::Die_KnockOut);
			return;
		}
	}
	else if (IsWalking())
	{
		if (dynamic_cast<CGoomba*>(e->obj) || dynamic_cast<CKoopas*>(e->obj))
		{
			if (e->nx != 0)
			{
				vx = -vx;
				nx = DirectionFromVelocity(vx);
			}
			return;
		}
	}

	if (!e->obj->IsBlocking()) return;

	if (e->ny != 0)
	{
		vy = 0;
	}
	else if (e->nx != 0)
	{
		vx = -vx;
		nx = DirectionFromVelocity(vx);
	}
}

void CKoopas::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt;
	vx += ax * dt;

	if (y > 1000)
	{
		isDeleted = true;
		return;
	}

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CKoopas::Render()
{
	int aniId = ID_ANI_KOOPAS_WALKING;

	if (state == ToInt(KoopasState::ShellIdle))
		aniId = ID_ANI_KOOPAS_SHELL_IDLE;
	else if (state == ToInt(KoopasState::ShellMoving))
		aniId = ID_ANI_KOOPAS_SHELL_MOVING;
	else if (state == ToInt(KoopasState::Die_KnockOut))
		aniId = ID_ANI_KOOPAS_DIE_KNOCKOUT;

	CAnimations::GetInstance()->Get(aniId)->Render(x, y, -nx, state == ToInt(KoopasState::Die_KnockOut) ? -1 : 1);
	//RenderBoundingBox();
}

void CKoopas::Kick(int direction)
{
	if (direction == 0) direction = 1;
	nx = direction;
	SetState(KoopasState::ShellMoving);
}

void CKoopas::SetState(KoopasState state)
{
	int previousState = this->state;
	CGameObject::SetState(ToInt(state));

	switch (state)
	{
	case KoopasState::Walking:
		vx = -KOOPAS_WALKING_SPEED;
		vy = 0;
		ay = KOOPAS_GRAVITY;
		nx = -1;
		break;
	case KoopasState::ShellIdle:
		vx = 0;
		vy = 0;
		ay = KOOPAS_GRAVITY;
		if (previousState == ToInt(KoopasState::Walking))
			y += (KOOPAS_BBOX_HEIGHT - KOOPAS_SHELL_BBOX_HEIGHT) / 2;
		break;
	case KoopasState::ShellMoving:
		vx = nx * KOOPAS_SHELL_SPEED;
		ay = KOOPAS_GRAVITY;
		break;
	case KoopasState::Die_KnockOut:
		vy = -0.5f;
		ay = KOOPAS_GRAVITY;
		break;
	}
}

LPGAMEOBJECT CKoopas::CreateFromTokens(const vector<string>& tokens)
{
	float x = (float)atof(tokens[1].c_str());
	float y = (float)atof(tokens[2].c_str());

	return new CKoopas(x, y);
}
