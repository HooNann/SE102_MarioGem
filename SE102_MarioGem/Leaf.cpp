#include "Leaf.h"

CLeaf::CLeaf(float x, float y) : CGameObject(x, y)
{
	this->vx = 0;
	this->vy = 0;
}

void CLeaf::Render()
{
	CAnimations::GetInstance()->Get(ID_ANI_ITEM_LEAF)->Render(x, y);
}

void CLeaf::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x - LEAF_BBOX_WIDTH / 2;
	top = y - LEAF_BBOX_HEIGHT / 2;
	right = left + LEAF_BBOX_WIDTH;
	bottom = top + LEAF_BBOX_HEIGHT;
}
