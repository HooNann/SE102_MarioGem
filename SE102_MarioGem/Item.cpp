#include "Item.h"

CItem::CItem(float x, float y) : CGameObject(x, y)
{
	appearStartY = y;
	appearTargetY = y;
	appearElapsed = 0;
}

void CItem::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (state != static_cast<int>(ItemState::Appearing))
		return;

	UpdateAppearing(dt, coObjects);
}

void CItem::UpdateAppearing(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	appearElapsed += dt;

	DWORD appearDuration = GetAppearDuration();
	float progress = 1.0f;
	if (appearDuration > 0)
	{
		progress = static_cast<float>(appearElapsed) / static_cast<float>(appearDuration);
		if (progress > 1.0f) progress = 1.0f;
	}

	y = appearStartY + (appearTargetY - appearStartY) * progress;

	if (progress >= 1.0f)
	{
		y = appearTargetY;
		OnAppearFinished();
	}
}

DWORD CItem::GetAppearDuration()
{
	return DEFAULT_ITEM_APPEAR_DURATION;
}

float CItem::GetAppearDistance()
{
	return DEFAULT_ITEM_APPEAR_DISTANCE;
}

void CItem::OnAppearFinished()
{
	SetState(ItemState::Active);
}

void CItem::SetState(ItemState state)
{
	CGameObject::SetState(static_cast<int>(state));

	switch (state)
	{
	case ItemState::Appearing:
		appearStartY = y;
		appearTargetY = y - GetAppearDistance();
		appearElapsed = 0;
		vx = 0;
		vy = 0;
		break;

	case ItemState::Active:
		vx = 0;
		vy = 0;
		break;
	}
}
