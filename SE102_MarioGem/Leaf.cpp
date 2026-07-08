#include "Leaf.h"

CLeaf::CLeaf(float x, float y) : CItem(x, y)
{
	SetState(ItemState::Appearing);
}

void CLeaf::Render()
{
	CAnimations::GetInstance()->Get(ID_ANI_ITEM_LEAF)->Render(x, y);
	//RenderBoundingBox();
}

void CLeaf::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x - LEAF_BBOX_WIDTH / 2;
	top = y - LEAF_BBOX_HEIGHT / 2;
	right = left + LEAF_BBOX_WIDTH;
	bottom = top + LEAF_BBOX_HEIGHT;
}

DWORD CLeaf::GetAppearDuration()
{
	return LEAF_APPEAR_DURATION;
}

float CLeaf::GetAppearDistance()
{
	return 8.0f + LEAF_BBOX_HEIGHT / 2;
}
