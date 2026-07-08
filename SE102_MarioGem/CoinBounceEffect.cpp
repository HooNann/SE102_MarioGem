#include "CoinBounceEffect.h"
#include "Animations.h"
#include "Sprites.h"

constexpr int ID_ANI_COIN_BOUNCE_EFFECT = 91019;
constexpr float COIN_BOUNCE_INITIAL_VY = -0.28f;
constexpr float COIN_BOUNCE_GRAVITY = 0.0012f;
constexpr DWORD COIN_BOUNCE_MAX_DURATION = 450;
constexpr DWORD POINT_POPUP_DURATION = 600;
constexpr float POINT_POPUP_SPEED_Y = -0.02f;
constexpr float COIN_BOUNCE_BBOX_SIZE = 16.0f;

CCoinBounceEffect::CCoinBounceEffect(float x, float y) : CGameObject(x, y)
{
	this->phase = Phase::CoinBounce;
	this->originY = y;
	this->vy = COIN_BOUNCE_INITIAL_VY;
	this->phaseStartTime = GetTickCount64();
}

void CCoinBounceEffect::StartPointPopup()
{
	phase = Phase::PointPopup;
	vy = POINT_POPUP_SPEED_Y;
	phaseStartTime = GetTickCount64();
}

void CCoinBounceEffect::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (phase == Phase::CoinBounce)
	{
		y += vy * dt;
		vy += COIN_BOUNCE_GRAVITY * dt;

		ULONGLONG elapsed = GetTickCount64() - phaseStartTime;
		if ((vy > 0.0f && y >= originY) || elapsed >= COIN_BOUNCE_MAX_DURATION)
		{
			y = originY;
			StartPointPopup();
		}
	}
	else
	{
		y += vy * dt;

		if (GetTickCount64() - phaseStartTime >= POINT_POPUP_DURATION)
			Delete();
	}
}

void CCoinBounceEffect::Render()
{
	if (phase == Phase::CoinBounce)
	{
		CAnimations::GetInstance()->Get(ID_ANI_COIN_BOUNCE_EFFECT)->Render(x, y);
	}
	else
	{
		LPSPRITE sprite = CSprites::GetInstance()->Get(ID_SPRITE_POINTS_100);
		if (sprite != NULL)
			sprite->Draw(x, y);
	}
}

void CCoinBounceEffect::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - COIN_BOUNCE_BBOX_SIZE / 2;
	t = y - COIN_BOUNCE_BBOX_SIZE / 2;
	r = l + COIN_BOUNCE_BBOX_SIZE;
	b = t + COIN_BOUNCE_BBOX_SIZE;
}
