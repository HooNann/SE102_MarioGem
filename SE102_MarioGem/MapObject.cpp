#include "MapObject.h"

CMapObject::CMapObject(float x, float y, int aniId) : CGameObject(x, y)
{
	this->aniId = aniId;
}

void CMapObject::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	// Map Object mặc định không làm gì cả (không rơi rớt)
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
	// Không có bounding box
	left = top = right = bottom = 0;
}
