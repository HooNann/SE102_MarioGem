#include "MapObject.h"

CMapObject::CMapObject(float x, float y, int aniId) : CGameObject(x, y)
{
	this->aniId = aniId;
}

void CMapObject::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
}

void CMapObject::Render()
{
	if (aniId != -1)
	{
		CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	}
}

void CMapObject::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	float w = 0, h = 0;
	if (aniId != -1)
	{
		LPANIMATION ani = CAnimations::GetInstance()->Get(aniId);
		if (ani)
		{
			w = (float)ani->GetSpriteWidth();
			h = (float)ani->GetSpriteHeight();
		}
	}
	left = x - w / 2;
	right = x + w / 2;
	top = y - h / 2;
	bottom = y + h / 2;
}
