#include "Blaster.h"
#include "Animations.h"

namespace
{
	constexpr int ToInt(BlasterState state)
	{
		return static_cast<int>(state);
	}
}

CBlaster::CBlaster(float x, float y) : CGameObject(x, y)
{
	stateTimer = GetTickCount64();
	SetState(BlasterState::Idle);
}

void CBlaster::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left  = x - BLASTER_BBOX_W / 2.0f;
	right = x + BLASTER_BBOX_W / 2.0f;

	bottom = y - BLASTER_BBOX_H / 2.0f;
	top    = bottom - BLASTER_BLAST_HEIGHT;
}

void CBlaster::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	ULONGLONG elapsed = GetTickCount64() - stateTimer;

	if (state == ToInt(BlasterState::Idle) && elapsed > BLASTER_IDLE_TIME)
		SetState(BlasterState::Firing);
	else if (state == ToInt(BlasterState::Firing) && elapsed > BLASTER_FIRE_TIME)
		SetState(BlasterState::Idle);
}

void CBlaster::Render()
{
	int aniId = (state == ToInt(BlasterState::Firing)) ? ID_ANI_BLASTER_FIRING : ID_ANI_BLASTER_IDLE;
	auto ani = CAnimations::GetInstance()->Get(aniId);
	if (ani) ani->Render(x, y);
	//RenderBoundingBox();
}

void CBlaster::SetState(BlasterState state)
{
	CGameObject::SetState(ToInt(state));
	stateTimer = GetTickCount64();
}

LPGAMEOBJECT CBlaster::CreateFromTokens(const vector<string>& tokens)
{
	float x = (float)atof(tokens[1].c_str());
	float y = (float)atof(tokens[2].c_str());
	return new CBlaster(x, y);
}
