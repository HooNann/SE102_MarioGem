#include "Item.h"

CItem::CItem(float x, float y, int type) : CGameObject(x, y)
{
	this->itemType = type;
	this->vy = 0;
	this->vx = 0;
}

void CItem::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	int aniId = -1;

	if (itemType == ITEM_TYPE_FLOWER)
		aniId = ID_ANI_ITEM_FLOWER;
	else if (itemType == ITEM_TYPE_LEAF)
		aniId = ID_ANI_ITEM_LEAF;

	if (aniId != -1)
		animations->Get(aniId)->Render(x, y);
}

void CItem::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	// Kích thước vật phẩm vuông 16x16 pixel chuẩn Mario 3
	left = x - 16.0f / 2;
	top = y - 16.0f / 2;
	right = left + 16.0f;
	bottom = top + 16.0f;
}