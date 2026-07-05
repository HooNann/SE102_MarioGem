#include "Mushroom.h"

CMushroom::CMushroom(float x, float y) : CGameObject(x, y)
{
	this->vx = 0;
	this->vy = 0;
}

void CMushroom::Render()
{
	CAnimations::GetInstance()->Get(ID_ANI_ITEM_MUSHROOM)->Render(x, y);
}

void CMushroom::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x - MUSHROOM_BBOX_WIDTH / 2;
	top = y - MUSHROOM_BBOX_HEIGHT / 2;
	right = left + MUSHROOM_BBOX_WIDTH;
	bottom = top + MUSHROOM_BBOX_HEIGHT;
}
