#include "RedVenus.h"
#include "VenusFireBall.h"
#include "Animations.h"
#include "Game.h"
#include "Mario.h"
#include "PlayScene.h"
#include <cmath>

namespace
{
	constexpr int ToInt(RedVenusState state)
	{
		return static_cast<int>(state);
	}
}

CRedVenus::CRedVenus(float x, float y) : CGameObject(x, y)
{
	baseY = y;
	topY = y - RED_VENUS_TRAVEL_DISTANCE;
	nx = 1;
	stateTimer = GetTickCount64();
	lastShotTime = 0;
	hasShotInReadyState = false;
	SetState(RedVenusState::Hidden);
}

bool CRedVenus::IsMarioFarEnough()
{
	auto scene = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene());
	if (!scene || !scene->GetPlayer()) return false;

	float marioX, marioY;
	scene->GetPlayer()->GetPosition(marioX, marioY);
	return fabsf(marioX - x) > RED_VENUS_SAFE_DISTANCE;
}

void CRedVenus::UpdateFacingMario()
{
	auto scene = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene());
	if (!scene || !scene->GetPlayer()) return;

	float marioX, marioY;
	scene->GetPlayer()->GetPosition(marioX, marioY);
	nx = marioX > x ? -1 : 1;
}

int CRedVenus::GetHeadAnimationId()
{
	auto scene = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene());
	if (!scene || !scene->GetPlayer()) return ID_ANI_RED_VENUS_HEAD_UP;

	float marioX, marioY;
	scene->GetPlayer()->GetPosition(marioX, marioY);
	return marioY <= y ? ID_ANI_RED_VENUS_HEAD_UP : ID_ANI_RED_VENUS_HEAD_DOWN;
}

void CRedVenus::ShootAtMario()
{
	auto scene = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene());
	if (!scene || !scene->GetPlayer()) return;

	float marioX, marioY;
	scene->GetPlayer()->GetPosition(marioX, marioY);

	float fireX = x;
	float fireY = y + RED_VENUS_FIREBALL_OFFSET_Y;
	scene->QueueSpawn(new CVenusFireBall(fireX, fireY, marioX, marioY));
	lastShotTime = GetTickCount64();
	hasShotInReadyState = true;
}

void CRedVenus::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (state == ToInt(RedVenusState::Hidden))
	{
		left = top = right = bottom = 0.0f;
		return;
	}

	left = x - RED_VENUS_BBOX_W / 2.0f;
	top = y - RED_VENUS_BBOX_H / 2.0f;
	right = x + RED_VENUS_BBOX_W / 2.0f;
	bottom = y + RED_VENUS_BBOX_H / 2.0f;
}

void CRedVenus::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	ULONGLONG now = GetTickCount64();
	ULONGLONG elapsed = now - stateTimer;
	if (state != ToInt(RedVenusState::Hidden))
		UpdateFacingMario();

	if (state == ToInt(RedVenusState::Hidden))
	{
		y = baseY;
		if (elapsed > RED_VENUS_HIDE_TIME && IsMarioFarEnough())
			SetState(RedVenusState::Rising);
	}
	else if (state == ToInt(RedVenusState::Rising))
	{
		y -= RED_VENUS_RISE_SPEED * dt;
		if (y <= topY)
		{
			y = topY;
			SetState(RedVenusState::Ready);
		}
	}
	else if (state == ToInt(RedVenusState::Ready))
	{
		if (!hasShotInReadyState && now - lastShotTime > RED_VENUS_SHOOT_COOLDOWN)
			ShootAtMario();

		if (elapsed > RED_VENUS_READY_TIME)
			SetState(RedVenusState::Lowering);
	}
	else if (state == ToInt(RedVenusState::Lowering))
	{
		y += RED_VENUS_LOWER_SPEED * dt;
		if (y >= baseY)
		{
			y = baseY;
			SetState(RedVenusState::Hidden);
		}
	}
}

void CRedVenus::Render()
{
	if (state == ToInt(RedVenusState::Hidden)) return;

	auto ani = CAnimations::GetInstance()->Get(GetHeadAnimationId());
	if (ani) ani->Render(x, y, nx);
	//RenderBoundingBox();
}

void CRedVenus::SetState(RedVenusState state)
{
	CGameObject::SetState(ToInt(state));
	stateTimer = GetTickCount64();

	if (state == RedVenusState::Ready)
		hasShotInReadyState = false;
}

LPGAMEOBJECT CRedVenus::CreateFromTokens(const vector<string>& tokens)
{
	float x = (float)atof(tokens[1].c_str());
	float y = (float)atof(tokens[2].c_str());
	return new CRedVenus(x, y);
}
