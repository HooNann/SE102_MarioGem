#include "Flower.h"

CFlower::CFlower(float x, float y) : CGameObject(x, y)
{
	this->vx = 0;
	this->vy = 0;
}

void CFlower::Render()
{
	CAnimations::GetInstance()->Get(ID_ANI_ITEM_FLOWER)->Render(x, y);
}

void CFlower::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x - FLOWER_BBOX_WIDTH / 2;
	top = y - FLOWER_BBOX_HEIGHT / 2;
	right = left + FLOWER_BBOX_WIDTH;
	bottom = top + FLOWER_BBOX_HEIGHT;
}
