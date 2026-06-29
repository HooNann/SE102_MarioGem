#include "Burner.h"
#include "Animations.h"

namespace
{
	constexpr int ToInt(BurnerState state)
	{
		return static_cast<int>(state);
	}
}

CBurner::CBurner(float x, float y) : CGameObject(x, y)
{
	stateTimer = GetTickCount64();
	SetState(BurnerState::Idle);
}

void CBurner::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left  = x - BURNER_BBOX_W / 2.0f;
	top   = y - BURNER_BBOX_H_IDLE / 2.0f;
	right = x + BURNER_BBOX_W / 2.0f;

	if (state == ToInt(BurnerState::Firing))
		bottom = top + BURNER_BBOX_H_IDLE + BURNER_BBOX_H_FIRE;
	else
		bottom = top + BURNER_BBOX_H_IDLE;
}

void CBurner::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	ULONGLONG elapsed = GetTickCount64() - stateTimer;

	if (state == ToInt(BurnerState::Idle) && elapsed > BURNER_IDLE_TIME)
		SetState(BurnerState::Firing);
	else if (state == ToInt(BurnerState::Firing) && elapsed > BURNER_FIRE_TIME)
		SetState(BurnerState::Idle);
}

void CBurner::Render()
{
	int aniId = (state == ToInt(BurnerState::Firing)) ? ID_ANI_BURNER_FIRING : ID_ANI_BURNER_IDLE;
	auto ani = CAnimations::GetInstance()->Get(aniId);
	if (ani) ani->Render(x, y);
	//RenderBoundingBox();
}

void CBurner::SetState(BurnerState state)
{
	CGameObject::SetState(ToInt(state));
	stateTimer = GetTickCount64();
}

LPGAMEOBJECT CBurner::CreateFromTokens(const vector<string>& tokens)
{
	float x = (float)atof(tokens[1].c_str());
	float y = (float)atof(tokens[2].c_str());
	return new CBurner(x, y);
}
