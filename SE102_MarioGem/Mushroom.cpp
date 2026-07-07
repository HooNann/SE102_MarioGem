#include "Mushroom.h"

CMushroom::CMushroom(float x, float y) : CItem(x, y)
{
	SetState(ItemState::Appearing);
}

void CMushroom::Render()
{
	CAnimations::GetInstance()->Get(ID_ANI_ITEM_MUSHROOM)->Render(x, y);
	//RenderBoundingBox();
}

void CMushroom::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x - MUSHROOM_BBOX_WIDTH / 2;
	top = y - MUSHROOM_BBOX_HEIGHT / 2;
	right = left + MUSHROOM_BBOX_WIDTH;
	bottom = top + MUSHROOM_BBOX_HEIGHT;
}

DWORD CMushroom::GetAppearDuration()
{
	return MUSHROOM_APPEAR_DURATION;
}

float CMushroom::GetAppearDistance()
{
	return 8.0f + MUSHROOM_BBOX_HEIGHT / 2;
}

void CMushroom::OnAppearFinished()
{
	CItem::OnAppearFinished();
	vx = MUSHROOM_WALKING_SPEED;
	vy = 0;
}

void CMushroom::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (state == static_cast<int>(ItemState::Appearing))
	{
		CItem::Update(dt, coObjects);
		return;
	}

	vy += MUSHROOM_GRAVITY * dt;

	if (y > MUSHROOM_DELETE_Y)
	{
		Delete();
		return;
	}

	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CMushroom::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
}

void CMushroom::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (!e->obj->IsBlocking()) return;

	if (e->ny != 0)
	{
		vy = 0;
	}
	else if (e->nx != 0)
	{
		vx = -vx;
	}
}
