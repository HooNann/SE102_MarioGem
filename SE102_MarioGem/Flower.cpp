#include "Flower.h"

CFlower::CFlower(float x, float y) : CItem(x, y)
{
	SetState(ItemState::Appearing);
}

void CFlower::Render()
{
	CAnimations::GetInstance()->Get(ID_ANI_ITEM_FLOWER)->Render(x, y);
	//RenderBoundingBox();
}

void CFlower::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x - FLOWER_BBOX_WIDTH / 2;
	top = y - FLOWER_BBOX_HEIGHT / 2;
	right = left + FLOWER_BBOX_WIDTH;
	bottom = top + FLOWER_BBOX_HEIGHT;
}

DWORD CFlower::GetAppearDuration()
{
	return FLOWER_APPEAR_DURATION;
}

float CFlower::GetAppearDistance()
{
	return 8.0f + FLOWER_BBOX_HEIGHT / 2;
}
 