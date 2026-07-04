#include "Canon.h"
#include "CannonBall.h"
#include "Animations.h"
#include "Game.h"
#include "PlayScene.h"
#include "SoundEvents.h"
#include "SoundSubject.h"

namespace
{
	constexpr int ToInt(CanonState s) { return static_cast<int>(s); }
}

CCanon::CCanon(float x, float y, int direction) : CGameObject(x, y)
{
	this->direction = direction;
	stateTimer = GetTickCount64();
	SetState(CanonState::Idle);
}

void CCanon::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left   = x - CANON_BBOX_W / 2.0f;
	top    = y - CANON_BBOX_H / 2.0f;
	right  = x + CANON_BBOX_W / 2.0f;
	bottom = y + CANON_BBOX_H / 2.0f;
}

void CCanon::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	ULONGLONG elapsed = GetTickCount64() - stateTimer;

	if (state == ToInt(CanonState::Idle) && elapsed > CANON_FIRE_INTERVAL)
	{
		auto scene = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene());
		if (scene)
		{
			float muzzleX = x + direction * CANON_MUZZLE_OFFSET_X;
			float muzzleY = y + CANON_MUZZLE_OFFSET_Y;
			scene->QueueSpawn(new CCannonBall(muzzleX, muzzleY, direction * CANNONBALL_SPEED));
			CSoundSubject::GetInstance()->Notify(EVENT_CANNON);
		}
		SetState(CanonState::Fired);
	}
	else if (state == ToInt(CanonState::Fired) && elapsed > CANON_FIRED_TIME)
	{
		SetState(CanonState::Idle);
	}
}

void CCanon::Render()
{
	auto ani = CAnimations::GetInstance()->Get(ID_ANI_CANON);
	if (ani) ani->Render(x, y);
	//RenderBoundingBox();
}

void CCanon::SetState(CanonState s)
{
	CGameObject::SetState(ToInt(s));
	stateTimer = GetTickCount64();
}

LPGAMEOBJECT CCanon::CreateFromTokens(const vector<string>& tokens)
{
	float x         = (float)atof(tokens[1].c_str());
	float y         = (float)atof(tokens[2].c_str());
	int   direction = tokens.size() > 3 ? atoi(tokens[3].c_str()) : 1;
	return new CCanon(x, y, direction);
}
